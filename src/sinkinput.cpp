#include <QSharedData>

#include "sinkinput.h"


class SinkInputData : public QSharedData
{
public:
    uint32_t index;
    QString name;
    uint32_t client;
    uint32_t sink;
};

SinkInput::SinkInput(uint32_t index, const QString &name, uint32_t client, uint32_t sink) :
    data(new SinkInputData)
{
    data->index = index;
    data->name = name;
    data->client = client;
    data->sink = sink;
}

SinkInput::SinkInput(const SinkInput &other) :
    data(other.data)
{
}

SinkInput::~SinkInput()
{
}

SinkInput &SinkInput::operator=(const SinkInput &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

bool SinkInput::operator==(const SinkInput &other)
{
    return index() == other.index();
}

bool SinkInput::operator!=(const SinkInput &other)
{
    return index() != other.index();
}

uint32_t SinkInput::index() const   { return data->index; }
QString SinkInput::name() const     { return data->name; }
uint32_t SinkInput::client() const  { return data->client; }
uint32_t SinkInput::sink() const    { return data->sink; }
