#ifndef TABLEDESIGNER_H
#define TABLEDESIGNER_H

#include <QMainWindow>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QCloseEvent>

#include "tablecolumneditor.h"

namespace Ui {
class TableEditor;
}

class TableEditor : public QMainWindow
{
    Q_OBJECT

    QJsonArray _columnMetas;
    QVector<TableColumnEditor*> _columns;
    QSqlDatabase _db;
    QString _name;
    QString _origName;
    TableColumnEditor* _index;

public:
    explicit TableEditor(QSqlDatabase db, QString tableName, QWidget *parent = 0);
    ~TableEditor();

    void closeEvent(QCloseEvent *event)
    {
        emit exiting();
        event->accept();
    }

public slots:
    void show()
    {
        noIndex();

        initFromTableName(_name);

        QMainWindow::show();
    }

private slots:
    void addColumn(QJsonObject meta = QJsonObject());
    bool validColumnName(TableColumnEditor* c, QString name);
    void indexChanged(TableColumnEditor* c);
    void noIndex();
    void deleteColumn(TableColumnEditor *c);
    void setTableName();
    void saveAndExit();

    bool updateTable(QString table, QJsonArray newTable);

    void showError(const QSqlError &err)
    {
        showMessage("Error executing database statement: " + err.text(), "Database Error");
    }

    void showMessage(const QString& msg, const QString& head)
    {
        QMessageBox::critical(this, head, msg);
    }

private:
    Ui::TableEditor *ui;

    void initFromTableName(QString name);

signals:
    void setIndex(TableColumnEditor* c);
    void exiting();

};

#endif // TABLEDESIGNER_H
