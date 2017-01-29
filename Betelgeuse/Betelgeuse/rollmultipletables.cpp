#include "rollmultipletables.h"
#include "ui_rollmultipletables.h"
#include "multiplerolltableresults.h"

#include <functional>

RollMultipleTables::RollMultipleTables(QSqlDatabase db, QWidget *parent) :
    QMainWindow(parent), _db(db),
    ui(new Ui::RollMultipleTables)
{
    ui->setupUi(this);

    connect(ui->button_add, SIGNAL(clicked(bool)), this, SLOT(addRollTable()));
    connect(ui->button_cancel, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->button_roll, SIGNAL(clicked(bool)), this, SLOT(roll()));
}

RollMultipleTables::~RollMultipleTables()
{
    delete ui;
}

void RollMultipleTables::addRollTable()
{
    RollTableSelector* select = new RollTableSelector(this);
    connect(select, &RollTableSelector::removed, std::bind(&RollMultipleTables::removeRollTable, this, select));
    selections.push_back(select);
    ui->layout_selectedTables->addWidget(select);
    select->setOptions(rollTableNames);
}

void RollMultipleTables::removeRollTable(RollTableSelector* ptr)
{
    selections.removeAll(ptr);
    ui->layout_selectedTables->removeWidget(ptr);
    ptr->deleteLater();
}

void RollMultipleTables::roll()
{
    QStringList tables;
    for(RollTableSelector* s : selections)
        if(s->selection().size())
            tables.push_back(s->selection());

    MultipleRollTableResults* results = new MultipleRollTableResults(tables);
    connect(results, SIGNAL(closing()), results, SLOT(deleteLater()));

    for(int i=0; i<tables.size(); i++)
    {
        QSqlQuery itemQuery = query("Select rti.item from " +
                                     ROLLTABLES_TABLE + " as rtt join " + ITEMSINROLLTABLE_TABLE + " as irt on rtt.id = irt.tableId " +
                                    " join " + ROLLTABLEITEMS_TABLE + " as rti on irt.itemId = rti.id " +
                                    "  where rtt.name = '" + tables[i] + "';", _db);
        QVector<QString> choices;
        while(itemQuery.next())
            choices.push_back(itemQuery.value(0).toString());

        if(choices.size())
        {
            int rolls = selections[i]->rollCount();
            for(int j=0; j<rolls; j++)
            {
                results->addItemToTable(i, choices[rand()%choices.size()]);
            }
        }
    }

    results->show();
}
