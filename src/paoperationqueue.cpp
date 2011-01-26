#include "paoperationqueue.h"
#include "paoperation.h"

PAOperationQueue::PAOperationQueue(QObject *parent) :
    QObject(parent),
    context(0)
{
}

void PAOperationQueue::enqueue(PAOperation *op)
{
    if (isRunning())
        return;

    op->setParent(this);
    queue.enqueue(op);
}

void PAOperationQueue::exec(pa_context *c)
{
    if (isRunning())
        return;

    context = c;
    execHead();
}

void PAOperationQueue::onOperationError(int errno)
{
    context = 0;
    emit error(errno);
}

void PAOperationQueue::onOperationFinished()
{
    queue.removeFirst();
    execHead();
}

void PAOperationQueue::execHead()
{
    if (queue.isEmpty()) {
        context = 0;
        emit finished();
    } else {
        PAOperation *op = queue.head();
        connect(op, SIGNAL(error(int)), SLOT(onOperationError(int)));
        connect(op, SIGNAL(finished()), SLOT(onOperationFinished()));
        op->exec(context);
    }
}

PAOperationQueue& PAOperationQueue::operator<<(PAOperation *op)
{
    enqueue(op);
    return *this;
}
