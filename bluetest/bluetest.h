#ifndef _BLUETEST_H_
#define _BLUETEST_H_

#include <bluedevil/bluedevil.h>
#include <QObject>

using namespace BlueDevil;

class BlueTest : public QObject
{
	Q_OBJECT

public:
	explicit BlueTest();
	virtual ~BlueTest() {}

private:
	Adapter *adapter;

private slots:
	void deviceFound(Device *);
	void deviceDisappeared(Device *);
	void startDiscovery();
	void stopDiscovery();
};

#endif
