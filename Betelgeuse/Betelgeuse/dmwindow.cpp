#include "dmwindow.h"
#include "ui_dmwindow.h"
#include "initdb.h"
#include "modifiablecombolist.h"
#include "rolltableresult.h"
#include "tableeditor.h"

#include <QString>
#include <QFileDialog>
#include <QDebug>
#include <QtSql>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <algorithm>

DmWindow::DmWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DmWindow)
{
    ui->setupUi(this);

    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(this, "Unable to load database", "This program needs the SQLITE driver");

    // initialize the database
    QSqlError err = initDb(db);
    if (err.type() != QSqlError::NoError) {
        showError(err);
    }
    //Tab-specific event handler
    connect(ui->widget_tabs, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));

    //Tables tab
    scriptEdit = new generalScriptEditor(this);
    connect(ui->button_scripts, SIGNAL(clicked(bool)), scriptEdit, SLOT(show()));
    connect(scriptEdit, SIGNAL(runScript(QString)), this, SLOT(executeScript(QString)));
    connect(ui->list_tables, SIGNAL(currentTextChanged(QString)), this, SLOT(displayTableInfo(QString)));
    connect(ui->button_manageData, SIGNAL(clicked(bool)), this, SLOT(manageTableData()));
    connect(ui->button_deleteTable, SIGNAL(clicked(bool)), this, SLOT(deleteTable()));
    connect(ui->button_newTable, SIGNAL(clicked(bool)), this, SLOT(newTable()));
    connect(ui->button_modifyTable, SIGNAL(clicked(bool)), this, SLOT(modifyTable()));

    //Search tab
    generalSearchDia = new ChooseSearchDialog(db, this);
    generalSearchModel = new QSqlQueryModel(this);
    ui->table_generalSearch->setModel(generalSearchModel);
    connect(ui->button_generalSearch, SIGNAL(clicked(bool)), this, SLOT(generalSearch()));
    connect(generalSearchDia, &ChooseSearchDialog::searchReady,
    [this](QString q){
        qDebug() << "New general query: " << q;
        generalSearchModel->setQuery(query(q, db));
        if(generalSearchModel->lastError().type() != QSqlError::NoError)
            showError(generalSearchModel->lastError());
    });

    //Rolltables tab
    rollSearchDia = new ChooseSearchDialog(db, this);
    rollSearchModel = new QSqlQueryModel(this);
    rollTableModel = new QSqlQueryModel(this);
    rollTableProxy = new RollTableItemsProxyModel(rollTableModel, this);
    ui->table_rollSearch->setModel(rollSearchModel);
    ui->table_rollCurrent->setSortingEnabled(true);
    ui->table_rollCurrent->setModel(rollTableProxy);
    connect(ui->button_rollSearch, SIGNAL(clicked(bool)), this, SLOT(rolltableSearch()));
    connect(rollSearchDia, &ChooseSearchDialog::searchReady,
    [this](QString q){
        qDebug() << "New roll query: " << q;
        rollSearchModel->setQuery(query(q, db));
        if(rollSearchModel->lastError().type() != QSqlError::NoError)
            showError(rollSearchModel->lastError());
    });
    rollCombo = new ModifiableComboList;
    ui->layout_combo->addWidget(rollCombo);
    connect(rollCombo, SIGNAL(newItem(QString)), this, SLOT(newRolltable(QString)));
    connect(rollCombo, SIGNAL(deleteItem(QString)), this, SLOT(deleteRolltable(QString)));
    connect(rollCombo, SIGNAL(copyItem(QString,QString)), this, SLOT(copyRolltable(QString,QString)));
    connect(rollCombo, SIGNAL(nameChange(QString,QString)), this, SLOT(renameRolltable(QString,QString)));
    connect(rollCombo, SIGNAL(selectionChange(QString)), this, SLOT(rolltableSelected(QString)));
    connect(ui->button_addToRollTable, SIGNAL(clicked(bool)), this, SLOT(addSelectedToRolltable()));
    connect(ui->button_removeFromRollTable, SIGNAL(clicked(bool)), this, SLOT(removeSelectedFromRolltable()));
    connect(ui->button_rollRollTable, SIGNAL(clicked(bool)), this, SLOT(rollCurrentRollTable()));
    rollTableProxy->sort(0);

    changeTab(ui->widget_tabs->currentIndex());
}

DmWindow::~DmWindow()
{
    delete ui;
}

void DmWindow::showError(const QSqlError &err)
{
    showMessage("Error executing database statement: " + err.text(), "Database Error");
}

void DmWindow::showMessage(const QString& msg, const QString& head)
{
    QMessageBox::critical(this, head, msg);
}


/***************************************************************************
********************************TABLES**************************************
***************************************************************************/

void DmWindow::executeScript(QString script)
{
#ifndef DEVMODE
    for(QString s : SYSTEM_TABLES)
    {
        if(script.contains(s))
        {
            showMessage("Please do not modify built-in tables");
            return;
        }
    }
#endif
    qDebug() << "Executing sql script: " << script;
    if(query(script, db).lastError().type() != QSqlError::NoError)
        showError(db.lastError());

    refreshTableList();

}

void DmWindow::refreshTableList()
{
    QStringList tables = db.tables();
    if(foundTables != tables)
    {
#ifndef DEVMODE
        QStringList filteredTables;
        for(QString t : tables)
            if(!SYSTEM_TABLES.contains(t))
                filteredTables << t;
#else
        QStringList filteredTables = tables;
#endif
        qDebug() << "Tables found: " << filteredTables;

        ui->list_tables->clear();
        ui->list_tables->addItems(filteredTables);

        foundTables = tables;
    }
    else
    {
        if(ui->list_tables->currentRow() >= 0)
            displayTableInfo(ui->list_tables->currentItem()->text());
    }
}

void DmWindow::changeTab(int tab)
{
    qDebug() << "Changed to tab " << tab;

    switch(tab)
    {
        case 0: refreshTableList(); break;
        case 2: refreshRollTableList(); break;
    }
}

void DmWindow::displayTableInfo(QString table)
{
    qDebug() << "Displaying data for table " << table;
    QSqlRecord rec = db.driver()->record(table);

    ui->table_currentTable->setRowCount(2);
    ui->table_currentTable->setColumnCount(rec.count());
    for(int i=0; i<rec.count(); i++)
    {
        ui->table_currentTable->setItem(0, i, new QTableWidgetItem(rec.fieldName(i)));
        ui->table_currentTable->setItem(1, i, new QTableWidgetItem(QMetaType::typeName(rec.field(i).type())));
    }

    selectedTable = table;
}

void DmWindow::manageTableData()
{
    qDebug() << "Displaying table: " << selectedTable;
    if(selectedTable != "")
    {
        if(!managedTables.contains(selectedTable))
        {
            managedTables[selectedTable] = new tableManager(selectedTable, db, this);
            managedTables[selectedTable]->setWindowTitle("Table Manager: " + selectedTable);
        }
        managedTables[selectedTable]->show();
    }
}

void DmWindow::newTable()
{
    QString name;
    int i=0;
    while(db.tables().contains("NewTable"+QString::number(i))) i++;
    name = "NewTable" + QString::number(i);

    query("create table " + name + "(defaultColumn int primary key);", db);

    QJsonArray meta{QJsonObject{{"name", "defaultColumn"}, {"type", "int"}, {"index", true}}};
    QString metaString = QJsonDocument(meta).toJson(QJsonDocument::Compact);
    query("insert into " + TABLEMETADATA_TABLE + " values ('" + name + "', '" + metaString + "')", db);

    refreshTableList();

    TableEditor* e = new TableEditor(db, name, this);
    connect(e, &TableEditor::exiting, e, &TableEditor::deleteLater);
    connect(e, &TableEditor::exiting, this, &DmWindow::refreshTableList);
    e->show();

}

void DmWindow::deleteTable()
{
    if(ui->list_tables->currentRow() < 0) return;
    QString name = ui->list_tables->currentItem()->text();

    query("drop table " + name + ";", db);
    query("delete from " + TABLEMETADATA_TABLE + " where tableName='" + name + "';", db);

    refreshTableList();
}

void DmWindow::modifyTable()
{
    if(ui->list_tables->currentRow() < 0) return;
    QString name = ui->list_tables->currentItem()->text();

    TableEditor* e = new TableEditor(db, name, this);
    connect(e, &TableEditor::exiting, e, &TableEditor::deleteLater);
    connect(e, &TableEditor::exiting, this, &DmWindow::refreshTableList);
    e->show();

}

/***************************************************************************
********************************ROLLTABLES**********************************
***************************************************************************/

void DmWindow::rolltableSearch()
{
    rollSearchDia->exec();
}

void DmWindow::refreshRollTableList()
{
    QSqlQuery q = query("select name from " + ROLLTABLES_TABLE, db);
    QStringList names;
    while(q.next())
    {
        names << q.value("name").toString();
    }
    rollCombo->setItems(names);
}

bool DmWindow::newRolltable(QString name)
{
    db.transaction();
    if(query("insert into " + ROLLTABLES_TABLE + "(name) values ('" + name + "');", db).lastError().type() != QSqlError::NoError)
    {
        db.rollback();
        return false;
    }
    db.commit();
    return true;
}

bool DmWindow::copyRolltable(QString oldName, QString copyName)
{
    db.transaction();
    if(!newRolltable(copyName))
    {
        db.rollback();
        return false;
    }

    QSqlQuery newTable = query("select id from " + ROLLTABLES_TABLE + " where name = '" + copyName + "';", db);
    if(!newTable.next())
    {
        db.rollback();
        return false;
    }

    int tableId = newTable.value("id").toInt();
    QSqlQuery currentTable = query("select irt.itemId from" +
                                   ITEMSINROLLTABLE_TABLE + " as irt join " + ROLLTABLES_TABLE + " as t on t.id = irt.tableId", db);
    while(currentTable.next())
    {
        if(query("insert into " + ITEMSINROLLTABLE_TABLE + QString("(tableId, itemId) values (%1, %2)").arg(tableId).arg(currentTable.value("item").toInt()), db).lastError().type() != QSqlError::NoError)
        {
            db.rollback();
            return false;
        }
    }
    db.commit();
    return true;
}

bool DmWindow::deleteRolltable(QString name)
{
    db.transaction();

    QSqlQuery table = query("select id from " + ROLLTABLES_TABLE + " where name = '" + name + "';", db);
    if(!table.next())
    {
        db.rollback();
        return false;
    }
    int tableId = table.value("id").toInt();
    if(query("delete from " + ITEMSINROLLTABLE_TABLE + " where tableId = " + QString::number(tableId), db).lastError().type() != QSqlError::NoError)
    {
        db.rollback();
        return false;
    }

    if(query("delete from " + ROLLTABLES_TABLE + " where name = '" + name + "';", db).lastError().type() != QSqlError::NoError)
    {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}

bool DmWindow::renameRolltable(QString oldName, QString newName)
{
    db.transaction();
    if(query("update " + ROLLTABLES_TABLE + " set name = '" + newName + "' where name = '" + oldName + "';", db).lastError().type() != QSqlError::NoError)
    {
        db.rollback();
        return false;
    }
    db.commit();
    return true;
}

void DmWindow::rolltableSelected(QString name)
{
    QSqlQuery q = query("select i.item, irt.id from " + ITEMSINROLLTABLE_TABLE + " as irt join " + ROLLTABLES_TABLE + " as t on irt.tableId = t.id"+
                        " join " + ROLLTABLEITEMS_TABLE + " as i on i.id = irt.itemId where t.name = '" + name + "';", db);
    rollTableModel->setQuery(q);
    ui->table_rollCurrent->setColumnHidden(1, true);
}

void DmWindow::addSelectedToRolltable()
{
    QSqlQuery q = query("select id from " + ROLLTABLES_TABLE + " where name = '" + rollCombo->currentIndex() + "';", db);
    if(q.next())
    {
        int tableId = q.value("id").toInt();
        QModelIndexList selected = ui->table_rollSearch->selectionModel()->selectedRows();
        for(QModelIndex i : selected)
        {
            int r = i.row();
            QSqlRecord info = rollSearchModel->record(r);
            QString stringified = "";

            for(int f = 0; f < info.count(); f++)
            {
                qDebug() << info.field(f);
                stringified += info.field(f).value().toString() + " : ";
            }

            QSqlQuery find = query("select * from " + ROLLTABLEITEMS_TABLE + " where item = '" + stringified + "';", db);
            if(!find.next())
                query("insert into " + ROLLTABLEITEMS_TABLE + " (item) values('" + stringified + "');", db);
            find = query("select * from " + ROLLTABLEITEMS_TABLE + " where item = '" + stringified + "';", db);
            if(find.next())
            {
                int itemId = find.value("id").toInt();
                query("insert into " + ITEMSINROLLTABLE_TABLE + "(tableId, itemId) values(" + QString::number(tableId) + ", '" + QString::number(itemId) + "');", db);
            }
        }
    }
    rolltableSelected(rollCombo->currentIndex());
}

void DmWindow::removeSelectedFromRolltable()
{
    QModelIndexList selected = ui->table_rollCurrent->selectionModel()->selectedRows();
    for(QModelIndex s : selected)
    {
        int r = s.row();
        QMap<int, QVariant> item = rollTableProxy->itemData(rollTableProxy->index(r, 1));
        int recId = item[0].toInt();
        //int recId = rec.field("id").value().toInt();
        query("delete from " + ITEMSINROLLTABLE_TABLE + " where id = " + QString::number(recId) + ";", db);
    }

    rolltableSelected(rollCombo->currentIndex());
}

void DmWindow::rollCurrentRollTable()
{
    RollTableResult* result = new RollTableResult(this);
    connect(result, SIGNAL(closing()), result, SLOT(deleteLater()));

    int items = ui->count_rollRollTable->value();
    for(int i=0; i< items; i++)
    {
        int index = rand()%rollTableModel->rowCount();
        result->addItem(rollTableModel->record(index).field("item").value().toString());
    }

    result->setWindowTitle("Roll result: " + QString::number(items) + " items from " + rollCombo->currentIndex());

    result->show();
}


/***************************************************************************
********************************GENERALSEARCH*******************************
***************************************************************************/

void DmWindow::generalSearch()
{
    generalSearchDia->exec();
}
