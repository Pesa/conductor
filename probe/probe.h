#ifndef PROBE_H
#define PROBE_H

#include <QDBusInterface>
#include <QObject>

class Probe : public QObject
{
    Q_OBJECT

public:
    explicit Probe(QObject *parent = 0);

    bool isDiscovering() const { return discovering; }

public slots:
    bool startDiscovery();
    bool stopDiscovery();

private:
    bool initAdapter();
    bool initManager();

    bool discovering;
    QDBusInterface *adapter;
    QDBusInterface *manager;
};

#endif // PROBE_H
