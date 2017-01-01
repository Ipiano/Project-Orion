#include "dmwindow.h"
#include <QApplication>
#include <QLoggingCategory>
#include <algorithm>
#include <ctime>

int main(int argc, char *argv[])
{
    srand(time(NULL));

    QApplication a(argc, argv);

    DmWindow w;
    w.show();

    return a.exec();
}
