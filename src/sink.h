#ifndef SINK_H
#define SINK_H

#include <QSharedDataPointer>
#include <QString>

#include <pulse/def.h>

class SinkData;

class Sink
{
public:
    explicit Sink(uint32_t index = -1, const QString &name = QString(),
                  const QString &desc = QString());
    Sink(const Sink &);
    ~Sink();

    Sink &operator=(const Sink &);
    bool operator==(const Sink &);
    bool operator!=(const Sink &);

    uint32_t index() const;
    QString name() const;
    QString description() const;

private:
    QSharedDataPointer<SinkData> data;
};

#endif // SINK_H
