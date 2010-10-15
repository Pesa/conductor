#ifndef CLIENTVIEWER_H
#define CLIENTVIEWER_H

#include <QtDebug>

#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/operation.h>

class Client
{
private:
    QList<QString>* clients;

public:
    Client(){
        clients = new QList<QString>();
    }

    QList<QString>* get_clients();
    void determine_client_available(pa_context *);
    static void client_cb(pa_context *, const pa_client_info *, int , void *);
};

#endif
