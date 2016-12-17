#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>

#include "generalscripteditor.h"

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

    void DmWindow::showError(const QSqlError &err);

private slots:
    void executeScript(QString script);
    void changeTab(int newTab);
    void refreshTableList();
    void displayTableData(QString table);
};

#endif // MAINWINDOW_H
