//#include "mainwidget.h"
#include "modeselect.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //mainwidget w;
    modeSelect w;
    w.show();

    return a.exec();
}
