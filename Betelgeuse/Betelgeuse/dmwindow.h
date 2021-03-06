#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QSqlQueryModel>
#include <QHash>
#include <QSortFilterProxyModel>

#include "generalscripteditor.h"
#include "tablemanager.h"
#include "choosesearchdialog.h"
#include "rollmultipletables.h"
#include "tableeditor.h"
#include "dieroller.h"

namespace Ui {
class DmWindow;
}

class DmWindow : public QMainWindow
{
    Q_OBJECT

    class RollTableItemsProxyModel;

public:
    explicit DmWindow(QWidget *parent = 0);
    ~DmWindow();

private:
    Ui::DmWindow *ui;
    QSqlDatabase db;
    generalScriptEditor* scriptEdit;

    //Tables tab
    QStringList foundTables;
    QHash<QString, tableManager*> managedTables;
    QHash<QString, TableEditor*> editedTables;
    QString selectedTable = "";

    //Search tab
    ChooseSearchDialog* generalSearchDia;
    QSqlQueryModel* generalSearchModel;

    //Rolltables tab
    ModifiableComboList* rollCombo;
    ChooseSearchDialog* rollSearchDia;
    QSqlQueryModel* rollSearchModel;
    QSqlQueryModel* rollTableModel;
    RollTableItemsProxyModel* rollTableProxy;
    RollMultipleTables* rollMultipleWindow;

    //Dice tab
    ModifiableComboList* diceCombo;
    QVector<DieRoller*> currDice;

    void showError(const QSqlError &err);
    void showMessage(const QString& msg, const QString &head = "Warning");

private slots:
    //Tab change handler
    void changeTab(int newTab);

    //Tables tab
    void executeScript(QString script);
    void refreshTableList();
    void displayTableInfo(QString table);
    void manageTableData();
    void newTable();
    void deleteTable();
    void modifyTable();
    void openTableEditor(QString table);

    //Search tab
    void generalSearch();

    //Rolltables tab
    void rolltableSearch();
    bool newRolltable(QString name);
    bool copyRolltable(QString oldName, QString copyName);
    bool deleteRolltable(QString name);
    bool renameRolltable(QString oldName, QString newName);
    void rolltableSelected(QString name);
    void addSelectedToRolltable();
    void removeSelectedFromRolltable();
    void refreshRollTableList();
    void rollCurrentRollTable();

    //Dice tab
    void saveDiceSet();
    void loadDiceSet();
    void addDie(int sides = 0);
    void removeDie(DieRoller* d);
    bool newDiceSet(QString name);
    bool copyDiceSet(QString oldName, QString copyName);
    bool renameDiceSet(QString oldName, QString newName);
    bool deleteDiceSet(QString name);
    void refreshDiceSetsList();
    void clearDice();
};

class DmWindow::RollTableItemsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    QSqlQueryModel* source;

public:
    RollTableItemsProxyModel(QSqlQueryModel* s, QObject* parent = NULL) : QSortFilterProxyModel(parent), source(s)
    {
        setSourceModel(s);
    }

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
    {
        return source->record(source_left.row()).field(0).value().toString() <
                source->record(source_right.row()).field(0).value().toString();
    }
};

#endif // MAINWINDOW_H
