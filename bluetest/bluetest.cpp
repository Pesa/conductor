#include <QtDebug>
#include <QTimer>
#include "bluetest.h"

BlueTest::BlueTest()
: QObject()
{
	Manager *manager = Manager::self();
	if (manager->isBluetoothOperational()) {
		adapter = manager->defaultAdapter();
		connect(adapter, SIGNAL(deviceFound(Device*)), this, SLOT(deviceFound(Device*)));
		connect(adapter, SIGNAL(deviceDisappeared(Device*)), this, SLOT(deviceDisappeared(Device*)));
		startDiscovery();
	} else {
		qWarning() << "Bluetooth not available";
	}
}

void BlueTest::deviceFound(Device *dev)
{
	qDebug() << "found BT device :" << dev->name();
	qDebug() << "\taddress :" << dev->address();
	qDebug() << "\t    UBI :" << dev->UBI();
	if (!dev->isRegistered()) {
		if (dev->registerDevice())
			qDebug() << "=> device registered successfully";
		else
			qWarning() << "=> device registration failed";
	}
}

void BlueTest::deviceDisappeared(Device *dev)
{
	qDebug() << "device" << dev->name() << "disappeared";
}

void BlueTest::startDiscovery()
{
	qDebug() << "starting discovery...";
	adapter->startDiscovery();
	QTimer::singleShot(10000, this, SLOT(stopDiscovery()));
}

void BlueTest::stopDiscovery()
{
	adapter->stopDiscovery();
	qDebug() << "discovery stopped.\n";
	QTimer::singleShot(3000, this, SLOT(startDiscovery()));
}
