#ifndef ROLLTABLESELECTOR_H
#define ROLLTABLESELECTOR_H

#include <QWidget>

namespace Ui {
class RollTableSelector;
}

class RollTableSelector : public QWidget
{
    Q_OBJECT

public:
    explicit RollTableSelector(QWidget *parent = 0);
    ~RollTableSelector();
    int rollCount();
    QString selection();
    void setOptions(QStringList options);

private:
    Ui::RollTableSelector *ui;

signals:
    void removed();
};

#endif // ROLLTABLESELECTOR_H
