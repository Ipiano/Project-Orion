#ifndef MODIFIABLECOMBOLIST_H
#define MODIFIABLECOMBOLIST_H

#include <QWidget>

namespace Ui {
class ModifiableComboList;
}

class ModifiableComboList : public QWidget
{
    Q_OBJECT

    QString defaultItem = "New Item";

    QString oldName;
    QString newName;
    bool buffered = false;

public:
    explicit ModifiableComboList(QWidget *parent = 0);
    ~ModifiableComboList();

    QString currentIndex();

private:
    Ui::ModifiableComboList *ui;
    QString getUniqueItem(QString s);

private slots:
    void newButtonClicked();
    void copyButtonClicked();
    void deleteButtonClicked();
    void textChanged(QString text);

public slots:
    void setItems(QStringList items);
    void setItem(QString item, int number = -1);
    void setDefaultItem(QString text){defaultItem = text;}

signals:
    bool newItem(QString);
    bool copyItem(QString, QString);
    bool deleteItem(QString);
    bool nameChange(QString, QString);
    void selectionChange(QString);
};

#endif // MODIFIABLECOMBOLIST_H
