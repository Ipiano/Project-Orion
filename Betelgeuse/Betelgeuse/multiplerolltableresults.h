#ifndef MULTIPLEROLLTABLERESULTS_H
#define MULTIPLEROLLTABLERESULTS_H

#include <QMainWindow>
#include <QTableWidget>
#include <QCloseEvent>

namespace Ui {
class MultipleRollTableResults;
}

class MultipleRollTableResults : public QMainWindow
{
    Q_OBJECT

    QVector<QTableWidget*> tables;

public:
    explicit MultipleRollTableResults(QStringList tableNames, QWidget *parent = 0);
    ~MultipleRollTableResults();
    void addItemToTable(int table, QString item);
    void closeEvent(QCloseEvent *event)
    {
        emit closing();
        event->accept();
    }

private:
    Ui::MultipleRollTableResults *ui;

signals:
    void closing();
};

#endif // MULTIPLEROLLTABLERESULTS_H
