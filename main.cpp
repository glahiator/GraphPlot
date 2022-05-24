#include "GraphPlot.h"

#include <QApplication>

#include "snap7.h"

#ifdef OS_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

TS7Client *Client;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphPlot w;
    w.show();
    return a.exec();
}

