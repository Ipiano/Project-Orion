#ifndef INITDB_H
#define INITDB_H

#include <QtSql>
#include <QString>
#include <QVector>
#include <QDebug>

const QString QUERY_TABLE = "savedQueries";
const QString ROLLTABLEITEMS_TABLE = "rolltableItems";
const QString ROLLTABLES_TABLE = "rolltables";
const QString ITEMSINROLLTABLE_TABLE = "itemInRolltable";
const QString TABLEMETADATA_TABLE = "tableMetadata";
const QVector<QString> SYSTEM_TABLES{QUERY_TABLE, ROLLTABLES_TABLE, ROLLTABLEITEMS_TABLE, ITEMSINROLLTABLE_TABLE, TABLEMETADATA_TABLE, "sqlite_sequence"};

inline QSqlQuery query(QString text, QSqlDatabase& db)
{
    qDebug() << text;
    QSqlQuery q(text, db);
    if(q.lastError().type() != QSqlError::NoError)
        qDebug() << q.lastError().text();
    return q;
}

inline QSqlError initDb(QSqlDatabase& db)
{
    db = QSqlDatabase::addDatabase("QSQLITE", "OrionDatabase");
    db.setDatabaseName("OrionData.db");

    if (!db.open())
        return db.lastError();

    if(!db.tables().contains(QUERY_TABLE))
    {
        QSqlQuery q = query("Create table " + QUERY_TABLE + "(name text primary key, query text)", db);
        if(q.lastError().type() != QSqlError::NoError)
            return q.lastError();
    }

    if(!db.tables().contains(ROLLTABLES_TABLE))
    {
        QSqlQuery q = query("Create table " + ROLLTABLES_TABLE + "(id integer primary key autoincrement , name text unique)", db);
        if(q.lastError().type() != QSqlError::NoError)
            return q.lastError();
    }

    if(!db.tables().contains(ROLLTABLEITEMS_TABLE))
    {
        QSqlQuery q = query("Create table " + ROLLTABLEITEMS_TABLE + "(id integer primary key autoincrement , item text unique)", db);
        if(q.lastError().type() != QSqlError::NoError)
            return q.lastError();
    }

    if(!db.tables().contains(ITEMSINROLLTABLE_TABLE))
    {
        QSqlQuery q = query("Create table " + ITEMSINROLLTABLE_TABLE + "(id integer primary key autoincrement, tableId integer, itemId integer)", db);
        if(q.lastError().type() != QSqlError::NoError)
            return q.lastError();
    }

    if(!db.tables().contains(TABLEMETADATA_TABLE))
    {
        QSqlQuery q = query("Create table " + TABLEMETADATA_TABLE + "(tableName text, jsonMeta text)", db);
        if(q.lastError().type() != QSqlError::NoError)
            return q.lastError();
    }

    return QSqlError();
}

#endif // INITDB_H
