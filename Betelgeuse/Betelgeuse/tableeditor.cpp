#include "tableeditor.h"
#include "ui_tableeditor.h"
#include "initdb.h"

#include <QSqlQuery>
#include <QJsonDocument>
#include <Quuid>
#include <QRegularExpression>

TableEditor::TableEditor(QSqlDatabase db, QString tableName, QWidget *parent) :
    QMainWindow(parent), _db(db),
    ui(new Ui::TableEditor)
{
    ui->setupUi(this);

    noIndex();

    initFromTableName(tableName);

    connect(ui->button_newColumn, SIGNAL(pressed()), this, SLOT(addColumn()));

    connect(ui->button_cancel, &QPushButton::pressed, this, &TableEditor::exiting);
    connect(ui->button_cancel, &QPushButton::pressed, this, &TableEditor::close);
    connect(ui->edit_tableName, &QLineEdit::editingFinished, this, &TableEditor::setTableName);
    connect(ui->button_saveExit, &QPushButton::pressed, this, &TableEditor::saveAndExit);

    _name = _origName = tableName;
}

TableEditor::~TableEditor()
{
    delete ui;
}

void TableEditor::addColumn(QJsonObject meta)
{
    qDebug() << "New column from metadata: " << meta;

    TableColumnEditor* e = new TableColumnEditor(_db);
    ui->layout_columns->addWidget(e);
    _columns.push_back(e);

    connect(e, &TableColumnEditor::nameChanged, this, &TableEditor::validColumnName);
    connect(e, &TableColumnEditor::isIndex, this, &TableEditor::indexChanged);
    connect(e, &TableColumnEditor::notIndex, this, &TableEditor::noIndex);
    connect(e, &TableColumnEditor::deleted, this, &TableEditor::deleteColumn);
    connect(this, &TableEditor::setIndex, e, &TableColumnEditor::setIndexColumn);
    e->initFromData(meta);
}

void TableEditor::initFromTableName(QString name)
{
    ui->edit_tableName->setText(name);

    QSqlQuery q("select * from " + TABLEMETADATA_TABLE + " where tableName = '" + name + "';", _db);
    if(q.next())
    {
        qDebug() << "Loading table from metadata";
        QSqlRecord rec = q.record();
        QString json = rec.value("jsonMeta").toString();
        _columnMetas = QJsonDocument::fromJson(json.toLatin1()).array();

        for(QJsonValue v : _columnMetas)
            addColumn(v.toObject());
    }
    else
    {
        qDebug() << "No metadata for table " << name;
        QSqlRecord r = _db.driver()->record(name);
        for(int i=0; i < r.count(); i++)
        {
            QSqlField f = r.field(i);
            QJsonObject meta;
            meta["name"] = r.fieldName(i);
            QString type;
            QString typeName = QMetaType::typeName(f.type());
            if(typeName == "QString") type = "text";
            else if(typeName == "int") type = "int";
            else if(typeName == "real") type = "real";
            else type = "text";
            meta["type"] = type;

            addColumn(meta);
        }
    }
}

bool TableEditor::validColumnName(TableColumnEditor *c, QString name)
{
    qDebug() << "Checking if " << name << " is a valid column name";
    for(TableColumnEditor* col : _columns)
        if(c != col && name == col->getName()) return false;
    return true;
}

void TableEditor::indexChanged(TableColumnEditor *c)
{
    qDebug() << "Index set";
    ui->button_saveExit->setEnabled(true);
    ui->button_saveExit->setText("Save and Exit");
    _index = c;

    emit setIndex(c);
}

void TableEditor::noIndex()
{
    qDebug() << "No index set";
    ui->button_saveExit->setEnabled(false);
    ui->button_saveExit->setText("Select an index column");
}

void TableEditor::deleteColumn(TableColumnEditor* c)
{
    if(_columns.size() < 2) return;
    qDebug() << "Delete column";
    ui->layout_columns->removeWidget(c);
    c->deleteLater();
    _columns.remove(_columns.indexOf(c));
    if(c == _index) noIndex();
}

void TableEditor::setTableName()
{
    QString newName = ui->edit_tableName->text().replace(' ', '_');
    if(newName == _name) return;

    qDebug() << "Changing table name to " << newName;
    if(!_db.tables().contains(newName))
    {
        _name = newName;
    }
    ui->edit_tableName->setText(_name);
}

void TableEditor::saveAndExit()
{
    QStringList oldNames;
    for(QJsonValue v : _columnMetas)
        oldNames << v.toObject()["name"].toString();

    QJsonArray metas;
    for(TableColumnEditor* c : _columns)
        metas.push_back(c->getJson());

    qDebug() << "Saving table:\n" << _columnMetas << "\n" << metas;

    QString jsonMeta = QJsonDocument(metas).toJson(QJsonDocument::Compact);
    _db.transaction();
    if(query("update " + TABLEMETADATA_TABLE + " set tableName='" + _name +
          "', jsonMeta='" + jsonMeta + "' where tableName='" + _origName + "';", _db).numRowsAffected() < 1)
        query("insert into " + TABLEMETADATA_TABLE + " values('" + _name + "', '" + jsonMeta + "');", _db);

    if(_origName != _name)
        query("alter table " + _origName + " rename to " + _name, _db);

    if(!updateTable(_name, metas)) _db.rollback();
    else _db.commit();

    emit exiting();
    close();
}

bool TableEditor::updateTable(QString table, QJsonArray newTable)
{
    _db.transaction();

    QSqlQuery q;
    QString tmpName = table+QUuid::createUuid().toString().replace(QRegularExpression("{|}|-"), "_");
    QVector<QJsonObject> renamed;
    q = query("alter table " + table + " rename to " + tmpName, _db);
    if(q.lastError().type() != QSqlError::NoError)
    {
        showError(q.lastError());
        _db.rollback();
        return false;
    }

    QString newQuery = "create table " + table + "(";
    QString foreign = "";
    for(int i=0; i<newTable.size(); i++)
    {
        QJsonObject o = newTable[i].toObject();
        newQuery += o["name"].toString() + " ";
        newQuery += o["type"].toString() + " ";
        if(o["index"].toBool()) newQuery += "primary key ";
        if(o["foreign"].toBool())
        {
            foreign += ", foreign key(" + o["name"].toString() + ") references " +
            o["foreignTable"].toString() + "(" + o["foreignCol"].toString() + ")";
        }
        if(i < newTable.size()-1) newQuery += ", ";

        if(o.contains("oldName"))
            renamed.push_back(o);
    }
    newQuery += foreign + ")";

    q = query(newQuery, _db);
    if(q.lastError().type() != QSqlError::NoError)
    {
        showError(q.lastError());
        _db.rollback();
        return false;
    }

    QString select = " select ";
    newQuery = "insert into " + table + "(";
    for(int i=0; i<renamed.size(); i++)
    {
        QJsonObject& o = renamed[i];
        select += o["oldName"].toString() + " as " + o["name"].toString();
        newQuery += o["name"].toString();
        if(i < renamed.size()-1)
        {
            select += ", ";
            newQuery += ", ";
        }
    }
    newQuery += ") " + select;
    newQuery += " from " + tmpName;

    q = query(newQuery, _db);
    if(q.lastError().type() != QSqlError::NoError)
    {
        showError(q.lastError());
        _db.rollback();
        return false;
    }

    q = query("drop table " + tmpName, _db);
    if(q.lastError().type() != QSqlError::NoError)
    {
        showError(q.lastError());
        _db.rollback();
        return false;
    }

    _db.commit();

    return true;
}
