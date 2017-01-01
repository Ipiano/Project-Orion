#include "tablemanager.h"
#include "ui_tablemanager.h"

#include <QMessageBox>

tableManager::tableManager(QString table, QSqlDatabase database, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tableManager)
{
    ui->setupUi(this);

    model = new QSqlTableModel(ui->list_rows, database);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable(table);

    // Populate the model
    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    // Set the model and hide the ID column
    ui->list_rows->setModel(model);
    ui->list_rows->setSortingEnabled(true);

    connect(ui->button_save, SIGNAL(clicked(bool)), this, SLOT(submitAll()));
    connect(ui->button_revert, SIGNAL(clicked(bool)), this, SLOT(revertAll()));
    connect(ui->button_delete, SIGNAL(clicked(bool)), this, SLOT(deleteCurrent()));
    connect(ui->button_new, SIGNAL(clicked(bool)), this, SLOT(addNew()));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(dataChanged()));
    connect(ui->list_rows->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(dataChanged()));

    tableName = table;
    db = database;

    QSqlRecord rec = db.driver()->record(table);
    for(int i=0; i<rec.count(); i++)
    {
        QString header = rec.fieldName(i);
        model->setHeaderData(model->fieldIndex(header), Qt::Horizontal, header);
    }
}

tableManager::~tableManager()
{
    delete ui;
}

void tableManager::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Database Error",
                "Error executing database statement: " + err.text());
}

void tableManager::submitAll()
{
    if(!model->submitAll())
        showError(model->lastError());
}

void tableManager::revertAll()
{
    model->revertAll();
}

void tableManager::deleteCurrent()
{
    for(QModelIndex m : ui->list_rows->selectionModel()->selectedIndexes())
        model->removeRow(m.row());
}

void tableManager::addNew()
{
    if(!model->insertRow(0))
        showError(model->lastError());
}
