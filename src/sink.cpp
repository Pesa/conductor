#include <QSharedData>

#include "sink.h"


class SinkData : public QSharedData
{
public:
    uint32_t index;
    QString name;
    QString desc;
};

Sink::Sink(uint32_t index, const QString &name, const QString &desc) :
    data(new SinkData)
{
    data->index = index;
    data->name = name;
    data->desc = desc;
}

Sink::Sink(const Sink &other) :
    data(other.data)
{
}

Sink::~Sink()
{
}

Sink &Sink::operator=(const Sink &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

bool Sink::operator==(const Sink &other)
{
    return index() == other.index();
}

bool Sink::operator!=(const Sink &other)
{
    return index() != other.index();
}

uint32_t Sink::index() const        { return data->index; }
QString Sink::name() const          { return data->name; }
QString Sink::description() const   { return data->desc; }
