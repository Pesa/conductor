#include <QCoreApplication>
#include "bluetest.h"

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	BlueTest bt;
	return app.exec();
}
