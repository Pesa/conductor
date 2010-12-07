#ifndef CONFIG_H
#define CONFIG_H

#include <QHash>
#include <QSet>
#include <QString>

class Config
{
public:
    static int maxRetries() { return 2; }
    static int maxSimultaneousSpeakers() { return 3; }
    static QHash<QString, QString> probesAddresses();
    static QHash<QString, QSet<QString> > roomsTopology();
    static int rpcPort() { return 1234; }

private:
    Config() {}
};

#endif // CONFIG_H
