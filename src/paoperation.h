#ifndef PAOPERATION_H
#define PAOPERATION_H

#include <QObject>

#include <pulse/def.h>

class PAOperation : public QObject
{
    Q_OBJECT

public:
    PAOperation(QObject *owner, uint32_t index, QObject *parent = 0)
        : QObject(parent), _index(index), _owner(owner) {}

    uint32_t index() const { return _index; }
    QObject *owner() const { return _owner; }

private:
    uint32_t _index;
    QObject *_owner;
};

#endif // PAOPERATION_H
