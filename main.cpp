#include "graphity.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Graphity w;
    w.show();
    return a.exec();
}
