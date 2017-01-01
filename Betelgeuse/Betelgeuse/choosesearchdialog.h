#ifndef CHOOSESEARCHDIALOG_H
#define CHOOSESEARCHDIALOG_H

#include "modifiablecombolist.h"

#include <QDialog>
#include <QSqlDatabase>
#include <QPair>

namespace Ui {
class ChooseSearchDialog;
}

class ChooseSearchDialog : public QDialog
{
    Q_OBJECT

    ModifiableComboList* combo;
    QSqlDatabase db;
    QPair<QString, QString> dirty;

public:
    explicit ChooseSearchDialog(QSqlDatabase data, QWidget *parent = 0);
    ~ChooseSearchDialog();
    int exec(){loadQueryNames(); return QDialog::exec();}

private:
    Ui::ChooseSearchDialog *ui;

    QString createSearchQuery();
    void loadQueryNames();

private slots:
    bool newQuery(QString name);
    bool copyQuery(QString name, QString copyName);
    bool deleteQuery(QString name);
    bool queryNameChange(QString oldName, QString newName);
    void queryTextChange();
    void changeSelected(QString name);
    void flushChanges();

signals:
    void searchReady(QString);
};

#endif // CHOOSESEARCHDIALOG_H
