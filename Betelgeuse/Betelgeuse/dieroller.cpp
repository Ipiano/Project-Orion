#include "dieroller.h"
#include "ui_dieroller.h"
#include <algorithm>

DieRoller::DieRoller(int sides, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DieRoller)
{
    ui->setupUi(this);
    ui->spin_numSides->setValue(sides);

    connect(&_iterator, &QTimer::timeout, this, &DieRoller::iterateRoll);
    _iterator.setInterval(10);

    connect(ui->spin_numSides, &QSpinBox::editingFinished, [this](){sidesChanged(this->sides());});
    connect(ui->button_remove, &QPushButton::clicked, [this](){remove(this);});
    connect(ui->button_roll, &QPushButton::clicked, this, &DieRoller::roll);
}

DieRoller::~DieRoller()
{
    delete ui;
}

void DieRoller::iterateRoll()
{
    if(_currentIterations++ < _totalIterations)
    {
        int min = 1, max = sides();
        ui->label_result->setText(QString::number(rand()%max+min));
    }
    else
    {
        ui->label_result->setText(QString::number(_result));
        _iterator.stop();
    }
}

void DieRoller::roll()
{
    int min = 1, max = sides();
    _result = rand()%max + min;

    _currentIterations = 0;
    _totalIterations = rand()%100;
    _iterator.start();
}

int DieRoller::result()
{
    return _result;
}

int DieRoller::sides()
{
    return ui->spin_numSides->value();
}
