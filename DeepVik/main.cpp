#include "deepvik.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    deepvik w;
    w.show();
    return a.exec();
}
