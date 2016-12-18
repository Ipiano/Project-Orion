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

    connect(ui->button_save, SIGNAL(clicked(bool)), this, SLOT(submitAll()));
    connect(ui->button_revert, SIGNAL(clicked(bool)), this, SLOT(revertAll()));
    connect(ui->button_delete, SIGNAL(clicked(bool)), this, SLOT(deleteCurrent()));
    connect(ui->button_new, SIGNAL(clicked(bool)), this, SLOT(addNew()));
    connect(ui->button_sort, SIGNAL(clicked(bool)), this, SLOT(sort()));
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

    model->sort(sortColumn, sortOrder);
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
    dataChanged();
}

void tableManager::revertAll()
{
    model->revertAll();
    dataChanged();
}

void tableManager::deleteCurrent()
{
    for(QModelIndex m : ui->list_rows->selectionModel()->selectedIndexes())
        model->removeRow(m.row());
    dataChanged();
}

void tableManager::addNew()
{
    if(!model->insertRow(0))
        showError(model->lastError());
    dataChanged();
}

void tableManager::sort()
{
    if(model->isDirty())return;
    if(ui->list_rows->selectionModel()->selectedIndexes().size() != 1) return;

    QModelIndex selected = ui->list_rows->selectionModel()->selectedIndexes()[0];
    if(selected.column() != sortColumn)
    {
        qDebug() << "Sorting by new column: " << selected.column();
        sortColumn = selected.column();
        sortOrder = Qt::AscendingOrder;
    }else
    {
        sortOrder = (sortOrder == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder);
        qDebug() << "Changed sort order to " << sortOrder;
    }
    QSqlRecord selectedRow = model->record(selected.row());
    model->sort(selected.column(), sortOrder);

    qDebug() << "Current selection: " << selected.row() << "," << selected.column();
    for(int i=0; i<model->rowCount(); i++)
    {
        if(model->record(i) == selectedRow)
        {
            qDebug() << "Reselecting " << i << ", " << selected.column();
            QModelIndex here = model->index(i, selected.column());
            ui->list_rows->selectionModel()->select(here,QItemSelectionModel::Select);
            break;
        }
    }
}

void tableManager::dataChanged()
{
    qDebug() << "Updating sort button";
    if(model->isDirty())
    {
        ui->button_sort->setText("Save or revert pending changes before sorting");
        ui->button_sort->setFlat(true);
    }
    else
    {
        if(ui->list_rows->selectionModel()->selectedIndexes().size() != 1)
        {
            ui->button_sort->setText("Select exactly one cell to choose the column to sort by");
            ui->button_sort->setFlat(true);
        }
        else
        {
            ui->button_sort->setText("Sort by selected column (Click again to change sort direction)");
            ui->button_sort->setFlat(false);
        }
    }
}
