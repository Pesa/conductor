#include <QCoreApplication>

#include "probe.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Probe p;

    return app.exec();
}
