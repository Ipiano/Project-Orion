#include "multiplerolltableresults.h"
#include "ui_multiplerolltableresults.h"

#include <QLabel>
#include <QSpacerItem>

MultipleRollTableResults::MultipleRollTableResults(QStringList tableNames, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MultipleRollTableResults)
{
    ui->setupUi(this);

    for(QString q : tableNames)
    {
        QLabel* l = new QLabel(this);
        l->setText(q);

        QTableWidget* table = new QTableWidget(this);
        QHeaderView* head = new QHeaderView(Qt::Horizontal);
        head->setStretchLastSection(true);
        head->setVisible(false);
        head->setSortIndicatorShown(false);

        table->setColumnCount(1);
        table->setHorizontalHeader(head);
        ui->layout_scrollResults->addWidget(l);
        ui->layout_scrollResults->addWidget(table);
        ui->layout_scrollResults->addSpacerItem(new QSpacerItem(0, 0));

        tables.push_back(table);
    }
}

MultipleRollTableResults::~MultipleRollTableResults()
{
    delete ui;
}

void MultipleRollTableResults::addItemToTable(int table, QString item)
{
    if(tables.size() > table)
    {
        tables[table]->insertRow(0);
        tables[table]->setItem(0, 0, new QTableWidgetItem(item));
    }
}
