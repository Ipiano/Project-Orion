#ifndef CHOOSESEARCHDIALOG_H
#define CHOOSESEARCHDIALOG_H

#include "modifiablecombolist.h"
#include "searchparameter.h"

#include <QDialog>
#include <QSqlDatabase>
#include <QPair>
#include <QVector>

namespace Ui {
class ChooseSearchDialog;
}

class ChooseSearchDialog : public QDialog
{
    Q_OBJECT

    ModifiableComboList* combo;
    QSqlDatabase db;
    QPair<QString, QString> dirty;
    QVector<SearchParameter*> params;

public:
    explicit ChooseSearchDialog(QSqlDatabase data, QWidget *parent = 0);
    ~ChooseSearchDialog();
    int exec()
    {
        loadQueryNames();
        int res = QDialog::exec();
        flushChanges();
        clearParams();
        return res;
    }

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
    void clearParams();

signals:
    void searchReady(QString);
};

#endif // CHOOSESEARCHDIALOG_H
