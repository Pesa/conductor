#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("conductor");

    MainWindow w;
    w.show();

    return app.exec();
}
