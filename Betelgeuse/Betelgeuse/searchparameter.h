#ifndef SEARCHPARAMETER_H
#define SEARCHPARAMETER_H

#include <QWidget>

namespace Ui {
class SearchParameter;
}

class SearchParameter : public QWidget
{
    Q_OBJECT

    QString _label;

public:
    explicit SearchParameter(QString l, QWidget *parent = 0);
    ~SearchParameter();

    QString label(){return _label;}
    QString value();

private:
    Ui::SearchParameter *ui;
};

#endif // SEARCHPARAMETER_H
