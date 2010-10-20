#ifndef SINKINPUT_H
#define SINKINPUT_H

#include <QSharedDataPointer>
#include <QString>

#include <pulse/def.h>

class SinkInputData;

class SinkInput
{
public:
    explicit SinkInput(uint32_t index = -1, const QString &name = QString(),
                       uint32_t client = -1, uint32_t sink = -1);
    SinkInput(const SinkInput &);
    ~SinkInput();

    SinkInput &operator=(const SinkInput &);
    bool operator==(const SinkInput &);
    bool operator!=(const SinkInput &);

    uint32_t index() const;
    QString name() const;
    uint32_t client() const;
    uint32_t sink() const;

private:
    QSharedDataPointer<SinkInputData> data;
};

#endif // SINKINPUT_H
