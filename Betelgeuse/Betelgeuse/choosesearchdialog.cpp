#include "choosesearchdialog.h"
#include "ui_choosesearchdialog.h"
#include "modifiablecombolist.h"
#include "initdb.h"

#include <functional>
#include <QDebug>
#include <QSqlQuery>

using namespace std;

ChooseSearchDialog::ChooseSearchDialog(QSqlDatabase data, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    db(data),ui(new Ui::ChooseSearchDialog)
{
    ui->setupUi(this);

    combo = new ModifiableComboList;

    connect(combo, SIGNAL(newItem(QString)), this, SLOT(flushChanges()));
    connect(combo, SIGNAL(newItem(QString)), this, SLOT(newQuery(QString)));

    connect(combo, SIGNAL(copyItem(QString, QString)), this, SLOT(flushChanges()));
    connect(combo, SIGNAL(copyItem(QString, QString)), this, SLOT(copyQuery(QString, QString)));

    connect(combo, SIGNAL(deleteItem(QString)), this, SLOT(flushChanges()));
    connect(combo, SIGNAL(deleteItem(QString)), this, SLOT(deleteQuery(QString)));

    connect(combo, SIGNAL(nameChange(QString,QString)), this, SLOT(flushChanges()));
    connect(combo, SIGNAL(nameChange(QString,QString)), this, SLOT(queryNameChange(QString,QString)));

    connect(combo, SIGNAL(selectionChange(QString)), this, SLOT(flushChanges()));
    connect(combo, SIGNAL(selectionChange(QString)), this, SLOT(changeSelected(QString)));
    ui->layout_combo->addWidget(combo);

    connect(ui->button_search, SIGNAL(clicked(bool)), this, SLOT(flushChanges()));
    connect(ui->button_search, &QPushButton::clicked, [this](){emit searchReady(createSearchQuery());});
    connect(ui->button_search, SIGNAL(clicked(bool)), this, SLOT(close()));

    connect(ui->edit_query, SIGNAL(textChanged()), this, SLOT(queryTextChange()));
}

ChooseSearchDialog::~ChooseSearchDialog()
{
    delete ui;
}

void ChooseSearchDialog::loadQueryNames()
{
    dirty.first = dirty.second = "";

    QSqlQuery q = query("Select name from " + QUERY_TABLE, db);
    QStringList names;
    while(q.next())
        names << q.value("name").toString();
    combo->setItems(names);
}

bool ChooseSearchDialog::newQuery(QString name)
{
    return query("Insert into " + QUERY_TABLE + " values ('" + name + "', '');", db).lastError().type() == QSqlError::NoError;
}

bool ChooseSearchDialog::copyQuery(QString name, QString copyName)
{
    QSqlQuery q = query("Select query from " + QUERY_TABLE + " where name = '" + name + "';", db);
    if(q.next())
    {
        return query("Insert into " + QUERY_TABLE + " values ('" + copyName + "', '" + q.value(0).toString() + "');", db).lastError().type()
                == QSqlError::NoError;
    }
    return false;
}

bool ChooseSearchDialog::deleteQuery(QString name)
{
    return query("Delete from " + QUERY_TABLE + " where name = '" + name + "';", db).lastError().type() == QSqlError::NoError;
}

bool ChooseSearchDialog::queryNameChange(QString oldName, QString newName)
{
    return query("Update " + QUERY_TABLE + " set name = '" + newName + "' where name = '" + oldName + "';", db).lastError().type() == QSqlError::NoError;
}

void ChooseSearchDialog::queryTextChange()
{
    if(ui->edit_query->toPlainText() != dirty.second)
    {
        dirty.first = combo->currentIndex();
        dirty.second = ui->edit_query->toPlainText();
    }
}

void ChooseSearchDialog::flushChanges()
{
    if(dirty.first != "")
    {
        dirty.second.replace("'", "''");
        query("Update " + QUERY_TABLE + " set query = '" + dirty.second + "'" +
                        " where name = '" + dirty.first + "';", db);

        dirty.first = dirty.second = "";
    }
}

QString ChooseSearchDialog::createSearchQuery()
{
    QString q = ui->edit_query->toPlainText();
    if(q.contains("create", Qt::CaseInsensitive) || q.contains("update", Qt::CaseInsensitive)
            || q.contains("delete", Qt::CaseInsensitive) || q.contains("drop", Qt::CaseInsensitive)
            || !q.contains("select", Qt::CaseInsensitive)) return "";

    return q.replace('\n', ' ');
}

void ChooseSearchDialog::changeSelected(QString name)
{
    QSqlQuery q = query("Select query from " + QUERY_TABLE + " where name = '" + name + "';", db);
    ui->edit_query->setPlainText("");
    if(q.next())
    {
        ui->edit_query->setPlainText(q.value("query").toString());
        dirty.second = q.value("query").toString();
    }
    else
        qDebug() << "No query found for " << name << "???";
}

