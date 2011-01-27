#ifndef PAOPERATIONQUEUE_H
#define PAOPERATIONQUEUE_H

#include <QObject>
#include <QQueue>

class PAOperation;
struct pa_context;

class PAOperationQueue : public QObject
{
    Q_OBJECT

public:
    explicit PAOperationQueue(QObject *parent = 0);

    void enqueue(PAOperation *op);

    bool isRunning() const { return context != 0; }
    void setAbortOnFailure(bool abort) { abortOnFailure = abort; }

public slots:
    void exec(pa_context *c);

signals:
    void error(int errno);
    void finished();

private slots:
    void onOperationError(int errno);
    void onOperationFinished();

private:
    void execHead();

    bool abortOnFailure;
    pa_context *context;
    QQueue<PAOperation*> queue;
};

#endif // PAOPERATIONQUEUE_H
