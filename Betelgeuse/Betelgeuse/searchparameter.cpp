#include "searchparameter.h"
#include "ui_searchparameter.h"

SearchParameter::SearchParameter(QString l, QWidget *parent) :
    QWidget(parent), _label(l),
    ui(new Ui::SearchParameter)
{
    ui->setupUi(this);
    ui->label->setText(l);
}

SearchParameter::~SearchParameter()
{
    delete ui;
}

QString SearchParameter::value()
{
    return ui->edit->text();
}
