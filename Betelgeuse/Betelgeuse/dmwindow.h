#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QHash>

#include "generalscripteditor.h"
#include "tablemanager.h"

namespace Ui {
class DmWindow;
}

class DmWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DmWindow(QWidget *parent = 0);
    ~DmWindow();

private:
    Ui::DmWindow *ui;
    QSqlDatabase db;
    generalScriptEditor* scriptEdit;

    //Tables tab
    QTimer refreshTables;
    QStringList foundTables;
    QHash<QString, tableManager*> managedTables;
    QString selectedTable = "";

    void DmWindow::showError(const QSqlError &err);

private slots:
    void executeScript(QString script);
    void changeTab(int newTab);
    void refreshTableList();
    void displayTableInfo(QString table);
    void manageTableData();
};

#endif // MAINWINDOW_H
