#include "rolltableselector.h"
#include "ui_rolltableselector.h"

RollTableSelector::RollTableSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RollTableSelector)
{
    ui->setupUi(this);

    connect(ui->button_remove, SIGNAL(clicked(bool)), this, SIGNAL(removed()));
}

RollTableSelector::~RollTableSelector()
{
    delete ui;
}

int RollTableSelector::rollCount()
{
    return ui->spin_rollCount->value();
}

QString RollTableSelector::selection()
{
    return ui->combo_rollTable->currentText();
}

void RollTableSelector::setOptions(QStringList options)
{
    ui->combo_rollTable->clear();
    ui->combo_rollTable->addItems(options);
}
