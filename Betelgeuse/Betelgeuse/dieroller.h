#ifndef DIEROLLER_H
#define DIEROLLER_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class DieRoller;
}

class DieRoller : public QWidget
{
    Q_OBJECT

    int _currentIterations;
    int _totalIterations;
    int _result = 1;
    QTimer _iterator;

public:
    explicit DieRoller(int sides = 2, QWidget *parent = 0);
    ~DieRoller();
    int result();
    int sides();

signals:
    void sidesChanged(int);
    void remove(DieRoller*);

private:
    Ui::DieRoller *ui;

private slots:
    void iterateRoll();

public slots:
    void roll();
};

#endif // DIEROLLER_H
