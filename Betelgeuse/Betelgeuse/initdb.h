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
const QString DATABASEMETA_TABLE = "tableDatabasemeta";
const QVector<QString> SYSTEM_TABLES{DATABASEMETA_TABLE, QUERY_TABLE, ROLLTABLES_TABLE, ROLLTABLEITEMS_TABLE, ITEMSINROLLTABLE_TABLE, TABLEMETADATA_TABLE, "sqlite_sequence"};

const int DBVersion = 1;

inline QSqlQuery query(QString text, QSqlDatabase& db)
{
    qDebug() << text;
    QSqlQuery q(text, db);
    if(q.lastError().type() != QSqlError::NoError)
        qDebug() << q.lastError().text();
    return q;
}

inline QSqlError upgradeDatabaseVersion(QSqlDatabase db, int oldVersion = -1)
{
    qDebug() << "Upgrading database from version " + oldVersion;
    if(oldVersion < 0)
    {
        if(!db.tables().contains(QUERY_TABLE))
        {
            QSqlQuery q = query("Create table " + QUERY_TABLE + "(name text primary key, query text)", db);
            if(q.lastError().type() != QSqlError::NoError)
                return q.lastError();
            query("insert into " + QUERY_TABLE + " values('Select full table', 'select * from <<table>>')", db);
            query("insert into " + QUERY_TABLE + " values('Select based on single value', 'select * from <<table>> where <<column>>=<<value>>')", db);
        }

        if(!db.tables().contains(ROLLTABLES_TABLE))
        {
            QSqlQuery q = query("Create table " + ROLLTABLES_TABLE + "(id integer primary key , name text unique)", db);
            if(q.lastError().type() != QSqlError::NoError)
                return q.lastError();
        }

        if(!db.tables().contains(ROLLTABLEITEMS_TABLE))
        {
            QSqlQuery q = query("Create table " + ROLLTABLEITEMS_TABLE + "(id integer primary key, item text unique)", db);
            if(q.lastError().type() != QSqlError::NoError)
                return q.lastError();
        }

        if(!db.tables().contains(ITEMSINROLLTABLE_TABLE))
        {
            QSqlQuery q = query("Create table " + ITEMSINROLLTABLE_TABLE + "(id integer primary key, tableId integer, itemId integer)", db);
            if(q.lastError().type() != QSqlError::NoError)
                return q.lastError();
        }

        if(!db.tables().contains(TABLEMETADATA_TABLE))
        {
            QSqlQuery q = query("Create table " + TABLEMETADATA_TABLE + "(tableName text primary key, jsonMeta text)", db);
            if(q.lastError().type() != QSqlError::NoError)
                return q.lastError();
        }

        if(!db.tables().contains(DATABASEMETA_TABLE))
        {
            QSqlQuery q = query("Create table " + DATABASEMETA_TABLE + "(valueName text primary key, data text)", db);
            if(q.lastError().type() != QSqlError::NoError)
                return q.lastError();
            query("insert into " + DATABASEMETA_TABLE + " values('version', '" + QString::number(DBVersion) + "')", db);
        }
    }

    return QSqlError();
}

inline QSqlError initDb(QSqlDatabase& db)
{
    db = QSqlDatabase::addDatabase("QSQLITE", "OrionDatabase");
    db.setDatabaseName("OrionData.db");

    if (!db.open())
        return db.lastError();

    if(!db.tables().contains(DATABASEMETA_TABLE))
    {
        return upgradeDatabaseVersion(db);
    }
    else
    {
        QSqlQuery getVersion = query("select * from " + DATABASEMETA_TABLE + " where valueName='version'", db);
        getVersion.next();
        int versionNum = getVersion.record().field("version").value().toInt();
        return upgradeDatabaseVersion(db, versionNum);
    }

    return QSqlError();
}


#endif // INITDB_H
