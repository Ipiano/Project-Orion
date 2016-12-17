#ifndef YESNOCANCELDIALOG_H
#define YESNOCANCELDIALOG_H

#include <QDialog>

namespace Ui {
class YesNoCancelDialog;
}

class YesNoCancelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit YesNoCancelDialog(QString query, QWidget *parent = 0);
    ~YesNoCancelDialog();

    int exec();

private:
    int ret = 0;
    Ui::YesNoCancelDialog *ui;
};

#endif // YESNOCANCELDIALOG_H
