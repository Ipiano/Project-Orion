#ifndef ROLLTABLERESULT_H
#define ROLLTABLERESULT_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QString>

namespace Ui {
class RollTableResult;
}

class RollTableResult : public QMainWindow
{
    Q_OBJECT

public:
    explicit RollTableResult(QWidget *parent = 0);
    ~RollTableResult();
    void addItem(QString item);

private:
    Ui::RollTableResult *ui;
    void closeEvent(QCloseEvent *event)
    {
        emit closing();
        event->accept();
    }

signals:
    void closing();
};

#endif // ROLLTABLERESULT_H
