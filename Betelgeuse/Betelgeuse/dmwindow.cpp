#include "dmwindow.h"
#include "ui_dmwindow.h"
#include "initdb.h"

#include <QString>
#include <QFileDialog>
#include <QDebug>
#include <QtSql>
#include <QMessageBox>

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
        return;
    }

    //Tab-specific event handler
    connect(ui->widget_tabs, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));

    //Tables tab
    scriptEdit = new generalScriptEditor(this);
    connect(ui->button_scripts, SIGNAL(clicked(bool)), scriptEdit, SLOT(show()));
    connect(scriptEdit, SIGNAL(runScript(QString)), this, SLOT(executeScript(QString)));
    connect(&refreshTables, SIGNAL(timeout()), this, SLOT(refreshTableList()));
    connect(ui->list_tables, SIGNAL(currentTextChanged(QString)), this, SLOT(displayTableInfo(QString)));
    connect(ui->button_manageData, SIGNAL(clicked(bool)), this, SLOT(manageTableData()));

    refreshTables.setInterval(1000);
    refreshTables.setSingleShot(true);

    changeTab(0);
}

DmWindow::~DmWindow()
{
    delete ui;
}

void DmWindow::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Database Error",
                "Error executing database statement: " + err.text());
}

void DmWindow::executeScript(QString script)
{
    qDebug() << "Executing sql script: " << script;
    QSqlQuery q(db);
    if(!q.exec(script))
        showError(q.lastError());
}

void DmWindow::refreshTableList()
{
    QStringList tables = db.tables();
    if(foundTables != tables)
    {
        qDebug() << "Tables found: " << tables;

        ui->list_tables->clear();
        ui->list_tables->addItems(tables);

        foundTables = tables;
    }

    refreshTables.start();
}

void DmWindow::changeTab(int tab)
{
    qDebug() << "Changed to tab " << tab;
    refreshTables.stop();

    switch(tab)
    {
        case 0:
            refreshTableList();
        break;
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
            managedTables[selectedTable] = new tableManager(selectedTable, db, this);
        managedTables[selectedTable]->show();
    }
}
