#ifndef TABLECOLUMN_H
#define TABLECOLUMN_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QSqlDatabase>
#include <QJsonObject>

const QStringList DATA_TYPES{"integer", "real", "text"};
const QStringList DATA_TYPENAMES{"Integer", "Decimal", "Text"};

namespace Ui {
class TableColumnEditor;
}

class TableColumnEditor : public QWidget
{
    Q_OBJECT

    QJsonObject _meta
    {{"name", "newColumn"}, {"type", "int"}, {"index", false},
    {"foreign", false}, {"foreignTable", ""},
    {"foreignCol", ""}};
    QSqlDatabase _db;

public:
    explicit TableColumnEditor(QSqlDatabase db, QWidget *parent = 0);
    ~TableColumnEditor();

    QJsonObject getJson(){return _meta;}
    QString getName(){return _meta["name"].toString();}
    void initFromData(QJsonObject meta);

private:
    Ui::TableColumnEditor *ui;

signals:
    void isIndex(TableColumnEditor*);
    void notIndex();
    void deleted(TableColumnEditor*);
    bool nameChanged(TableColumnEditor*, QString);

public slots:
    void setIndexColumn(TableColumnEditor* index);
    bool setName(QString newName);

private slots:
    void changeReferenceState(bool isReference);
    void changeDatatype();
    void changeReferenceColumn();
    void changeReferenceTable();
};

#endif // TABLECOLUMN_H
