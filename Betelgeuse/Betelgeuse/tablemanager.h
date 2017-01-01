#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <QMainWindow>
#include <QtSql>

namespace Ui {
class tableManager;
}

class tableManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit tableManager(QString table, QSqlDatabase database, QWidget *parent = 0);
    ~tableManager();

private:
    Ui::tableManager *ui;
    QSqlTableModel *model;
    QSqlDatabase db;
    QString tableName;

    void showError(const QSqlError &err);

private slots:
    void submitAll();
    void deleteCurrent();
    void addNew();
    void revertAll();
};

#endif // TABLEMANAGER_H
