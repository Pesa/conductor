#include "paoperationqueue.h"
#include "paoperation.h"

PAOperationQueue::PAOperationQueue(QObject *parent) :
    QObject(parent),
    abortOnFailure(true),
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
    if (abortOnFailure) {
        emit error(errno);
        deleteLater();
    } else {
        onOperationFinished();
    }
}

void PAOperationQueue::onOperationFinished()
{
    queue.removeFirst();
    execHead();
}

void PAOperationQueue::execHead()
{
    if (queue.isEmpty()) {
        emit finished();
        deleteLater();
    } else {
        PAOperation *op = queue.head();
        connect(op, SIGNAL(error(int)), SLOT(onOperationError(int)));
        connect(op, SIGNAL(finished()), SLOT(onOperationFinished()));
        op->exec(context);
    }
}
