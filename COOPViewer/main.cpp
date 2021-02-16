#include "COOPViewer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    COOPViewer w;
    w.show();
    return a.exec();
}
