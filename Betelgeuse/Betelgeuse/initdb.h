#ifndef INITDB_H
#define INITDB_H

#include <QtSql>

QSqlError initDb(QSqlDatabase& db)
{
    db = QSqlDatabase::addDatabase("QSQLITE", "OrionDatabase");
    db.setDatabaseName("OrionData.db");

    if (!db.open())
        return db.lastError();

    return QSqlError();
}
#endif // INITDB_H
