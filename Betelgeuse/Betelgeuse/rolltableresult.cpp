#include "rolltableresult.h"
#include "ui_rolltableresult.h"

#include <QDebug>

RollTableResult::RollTableResult(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RollTableResult)
{
    ui->setupUi(this);

    ui->table_rollResult->setColumnCount(1);
}

RollTableResult::~RollTableResult()
{
    qDebug() << "Deleting roll result";
    delete ui;
}

void RollTableResult::addItem(QString item)
{
    ui->table_rollResult->insertRow(0);
    ui->table_rollResult->setItem(0, 0, new QTableWidgetItem(item));
}
