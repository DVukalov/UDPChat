#include "server.h"
#include <QApplication>
#include <wwWidgets/QwwListWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server w;    
    w.setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    w.show();

    return a.exec();
}
