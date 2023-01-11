#include "GraphPlot.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphPlot w;
    w.show();
    return a.exec();
}

