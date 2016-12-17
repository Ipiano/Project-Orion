#include "dmwindow.h"
#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DmWindow w;
    w.show();

    return a.exec();
}
