#include "RicoReader.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RicoReader w;
    w.show();
    return a.exec();
}
