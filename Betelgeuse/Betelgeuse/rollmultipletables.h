#ifndef ROLLMULTIPLETABLES_H
#define ROLLMULTIPLETABLES_H

#include <QMainWindow>
#include <QVector>
#include <QSqlDatabase>

#include "initdb.h"
#include "rolltableselector.h"

namespace Ui {
class RollMultipleTables;
}

class RollMultipleTables : public QMainWindow
{
    Q_OBJECT

    QSqlDatabase _db;
    QVector<RollTableSelector*> selections;
    QStringList rollTableNames;

public:
    explicit RollMultipleTables(QSqlDatabase db, QWidget *parent = 0);
    ~RollMultipleTables();

public slots:
    void show()
    {
        qDebug() << "Showing rolltables selection";
        rollTableNames.clear();
        QSqlQuery rollTablesQuery = query("select name from " + ROLLTABLES_TABLE, _db);
        while(rollTablesQuery.next())
            rollTableNames << rollTablesQuery.value(0).toString();

        qDebug() << "Rolltable options are " << rollTableNames;
        for(RollTableSelector* r : selections) r->setOptions(rollTableNames);

        QMainWindow::show();
    }

private:
    Ui::RollMultipleTables *ui;

private slots:
    void addRollTable();
    void removeRollTable(RollTableSelector* ptr);
    void roll();
};

#endif // ROLLMULTIPLETABLES_H
