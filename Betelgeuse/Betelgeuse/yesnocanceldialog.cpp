#include "yesnocanceldialog.h"
#include "ui_yesnocanceldialog.h"

#include <functional>
#include <QAbstractButton>
#include <QDebug>

using namespace std;

YesNoCancelDialog::YesNoCancelDialog(QString query, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
    ui(new Ui::YesNoCancelDialog)
{
    ui->setupUi(this);

    ui->label_query->setText(query);

    connect(ui->buttonBox, &QDialogButtonBox::clicked,
    [this](QAbstractButton* button)
    {
        qDebug() << button->text() << " pressed";
        if(button->text() == "&Yes") ret = 1;
        if(button->text() == "&No") ret = -1;
        if(button->text() == "Cancel") ret = 0;
    });
}

YesNoCancelDialog::~YesNoCancelDialog()
{
    delete ui;
}

int YesNoCancelDialog::exec()
{
    QDialog::exec();
    return ret;
}
