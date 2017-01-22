#include "tablecolumneditor.h"
#include "ui_tablecolumneditor.h"

#include <functional>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QSqlField>
#include <QDebug>

TableColumnEditor::TableColumnEditor(QSqlDatabase db, QWidget *parent) :
    QWidget(parent), _db(db),
    ui(new Ui::TableColumnEditor)
{
    ui->setupUi(this);

    connect(ui->check_isIndex, &QCheckBox::clicked, [this](bool checked){if(checked){ _meta["index"] = true; emit isIndex(this);} else emit notIndex();});
    connect(ui->button_delete, &QPushButton::pressed, [this](){emit deleted(this);});
    connect(ui->line_name, &QLineEdit::editingFinished,
    [this]()
    {
        setName(ui->line_name->text());
    });

    connect(ui->check_isRef, &QCheckBox::clicked, this, &TableColumnEditor::changeReferenceState);
    connect(ui->combo_type, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeDatatype()));
    connect(ui->combo_refColumn, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeReferenceColumn()));
    connect(ui->combo_refTable, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeReferenceTable()));

    ui->combo_type->addItems(DATA_TYPENAMES);
    ui->combo_refTable->addItems(db.tables());

    changeReferenceState(false);
    changeReferenceTable();
    changeDatatype();
    ui->combo_type->setCurrentIndex(0);

    qDebug() << "New column set up: " << _meta;
}

TableColumnEditor::~TableColumnEditor()
{
    delete ui;
}

void TableColumnEditor::setIndexColumn(TableColumnEditor *index)
{
    if(this != index)
    {
        _meta["index"] = false;
        ui->check_isIndex->setChecked(false);
    }
}

void TableColumnEditor::changeReferenceState(bool isReference)
{
    if(isReference)
    {
        ui->combo_refColumn->setEnabled(true);
        ui->combo_refTable->setEnabled(true);
        ui->combo_type->setEnabled(false);
        changeReferenceColumn();
    }
    else
    {
        ui->combo_refColumn->setEnabled(false);
        ui->combo_refTable->setEnabled(false);
        ui->combo_type->setEnabled(true);
    }

    _meta["foreign"] = isReference;
}

void TableColumnEditor::changeDatatype()
{
    if(ui->combo_type->currentIndex() < 0) return;
    QString type = DATA_TYPES[ui->combo_type->currentIndex()];

    _meta["type"] = type;
}

void TableColumnEditor::changeReferenceTable()
{
    QString table = ui->combo_refTable->currentText();
    QSqlRecord rec = _db.driver()->record(table);
    QStringList cols;

    for(int i=0; i<rec.count(); i++)
        cols << rec.fieldName(i);

    ui->combo_refColumn->clear();
    ui->combo_refColumn->addItems(cols);
    changeReferenceColumn();
}

void TableColumnEditor::changeReferenceColumn()
{
    QString table = ui->combo_refTable->currentText();
    QString col = ui->combo_refColumn->currentText();
    QSqlRecord rec = _db.driver()->record(table);
    QSqlField field = rec.field(rec.indexOf(col));

    QString fieldType =  QMetaType::typeName(field.type());
    qDebug() << "Column " << rec.indexOf(col) << " field type " << fieldType;
    if(fieldType == "int")
        ui->combo_type->setCurrentIndex(0);
    else if(fieldType == "double")
        ui->combo_type->setCurrentIndex(1);
    else if(fieldType == "QString")
        ui->combo_type->setCurrentIndex(2);
    else
    {
        ui->combo_type->setCurrentIndex(2);
        changeDatatype();
    }

    _meta["foreignTable"] = table;
    _meta["foreignCol"] = col;
}

bool TableColumnEditor::setName(QString newName)
{
    if(newName == "") newName = ui->line_name->text();

    newName.replace(' ', '_');
    bool good = nameChanged(this, newName);
    if(good)
    {
        _meta["name"] = newName;
    }

    ui->line_name->setText(_meta["name"].toString());

    return good;
}

void TableColumnEditor::initFromData(QJsonObject meta)
{
    qDebug() << "Initializing from metadata " << meta;

    QString originalName = meta["name"].toString();
    bool setOriginalName = setName(originalName);
    if(!setOriginalName || originalName == "")
    {
        int i=0;
        while(!setName("newColumn"+QString::number(i)))i++;
    }
    else
        _meta["oldName"] = originalName;

    ui->combo_type->setCurrentIndex(DATA_TYPES.indexOf(meta["type"].toString()));
    ui->check_isIndex->setChecked(meta["index"].toBool());
    if(meta["index"].toBool()) emit isIndex(this);
    ui->check_isRef->setChecked(meta["foreign"].toBool());

    QString table = meta["foreignTable"].toString();
    QString column = meta["foreignCol"].toString();
    if(_db.tables().contains(table))
    {
        ui->combo_refTable->setCurrentText(table);
        QSqlRecord rec = _db.record(table);
        if(rec.contains(column))
            ui->combo_refColumn->setCurrentText(column);
    }
}
