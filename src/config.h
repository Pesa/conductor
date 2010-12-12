#ifndef CONFIG_H
#define CONFIG_H

#include <QHash>
#include <QList>
#include <QSet>
#include <QString>

class Config
{
public:
    static int maxRetries() { return 2; }
    static int maxSimultaneousSpeakers() { return 2; }
    static QHash<QString, QString> probesAddresses();
    static QList<QString> roomsNames();
    static QHash<QString, QSet<QString> > roomsTopology();
    static int rpcPort() { return 1234; }
    static int updateInterval() { return 3000; }

private:
    Config() {}
};

#endif // CONFIG_H
