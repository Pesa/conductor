#ifndef PROBEINTERFACE_H
#define PROBEINTERFACE_H

#include <QxtRPCPeer>

class ProbeInterface : public QxtRPCPeer
{
    Q_OBJECT

public:
    ProbeInterface(const QString &name, const QString &address, QObject *parent = 0);

public slots:
    void connect();
    void disconnect();
    void addDevice(const QString &address);
    void removeDevice(const QString &address);
    void startDiscovery();
    void stopDiscovery();

signals:
    void connected(const QString &name);
    void disconnected(const QString &name);
    void error(const QString &name);
    void rssiChanged(const QString &name, const QString &device, int newRssi);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError errorCode);

private:
    QString _name;
    QString _addr;
};

#endif // PROBEINTERFACE_H
