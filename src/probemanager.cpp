#include <QxtSignalGroup>

#include "config.h"
#include "probemanager.h"
#include "probeinterface.h"

ProbeManager::ProbeManager(QObject *parent) :
    QObject(parent),
    addrs(Config::probesAddresses()),
    group(new QxtSignalGroup(this))
{
    QHashIterator<QString, QByteArray> i(addrs);
    while (i.hasNext()) {
        i.next();
        ProbeInterface *probe = new ProbeInterface(i.key(), i.value());
        probes[i.key()] = probe;
        group->addSignal(probe, SIGNAL(connected(QString)));
        connect(probe, SIGNAL(rssiChanged(QString,QString,int)), SIGNAL(rssiChanged(QString,QString,int)));
    }

    connect(group, SIGNAL(allSignalsReceived()), SIGNAL(ready()));
}

ProbeManager::~ProbeManager()
{
    qDeleteAll(probes);
    probes.clear();
}

void ProbeManager::connectToProbes()
{
    group->reset();
    foreach (ProbeInterface *i, probes)
        i->connect();
}

void ProbeManager::disconnectFromProbes()
{
    foreach (ProbeInterface *i, probes)
        i->disconnect();
}

void ProbeManager::startMonitoring(const QString &device)
{
    foreach (ProbeInterface *i, probes) {
        i->addDevice(device);
        i->startDiscovery();
    }
}

void ProbeManager::stopMonitoring(const QString &device)
{
    foreach (ProbeInterface *i, probes) {
        i->stopDiscovery();
        i->removeDevice(device);
    }
}
