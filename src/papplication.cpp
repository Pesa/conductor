#include <pulse/context.h>
#include <pulse/glib-mainloop.h>
#include <pulse/introspect.h>
#include <pulse/operation.h>

#include <QtDebug>

#include "sinkviewer.h"
#include "sinkinputviewer.h"
#include "clientviewer.h"
#include "moduleloader.h"


class Box
{
private:
    Sink *sink;
    SinkInput *sink_input;
    Client *client;
    Module *loader;

public:
    Box()
    {
        sink = new Sink();
        sink_input = new SinkInput();
        client = new Client();
        loader = new Module();
    }

    Sink get_sink() { return *sink; }
    SinkInput get_sink_input() { return *sink_input; }
    Client get_client() { return *client; }
    Module get_module() { return *loader; }

    void determine_server_connected_to(pa_context * c)
    {
        pa_operation * o;
        if (!(o = pa_context_get_server_info(c, server_cb, NULL))) {
            qWarning() << "pa_context_get_server_info() failed";
            return;
        }
        pa_operation_unref(o);
    }

    static void server_cb(pa_context *, const pa_server_info *i, void *)
    {
        qDebug() << "running @" << i->host_name << '\n';
    }

    static void context_state_cb(pa_context * c, void *b )
    {
        Box* box = static_cast<Box*>(b);

        pa_context_state_t state = pa_context_get_state(c);

        if (state == PA_CONTEXT_READY) {
            int local = pa_context_is_local(c);
            qDebug() << "connected to" << (local ? "local" : "remote") << "PulseAudio server" << pa_context_get_server(c);

            box->determine_server_connected_to(c);

            (box->sink)->determine_sink_available(c);

            (box->sink_input)->determine_sink_input_available(c);

            /*qDebug() << "lista dei nomi dei sink_input: ";
                        QMap<uint32_t, const char*>::iterator it;
                        for ( it = ((box->sink_input)->get_sinks_input()).begin(); it != ((box->sink_input)->get_sinks_input()).end(); ++it ) {
                                qDebug() << "nome sink_input" << it.value();
                        }*/


            (box->client)->determine_client_available(c);

            /*qDebug() << "lista dei nomi dei clients: ";
                        for (int j = 0; j < ((box->client)->get_clients())->size(); ++j) {
                                qDebug() << "client: " << ((box->client)->get_clients())->at(j);
                        }*/

            //server to connect to create a tunnel connection
            // fixed IP???
            const char* server = "server=127.0.0.1";

            //slave list for simultaneously playback
            QList<uint32_t>* list_slaves = new QList<uint32_t>();

            (box->loader)->load_module_tunnel(c, server);

            //(box->loader)->load_module_combine(c, *list_slaves);
            (box->sink_input)->move_audio_stream(c, 1);
        }

        else if (!PA_CONTEXT_IS_GOOD(state)) {
            qWarning() << "context PHAIL";
            pa_context_unref(c);
            //qApp->quit();
        }
    }

};


static void test_main()
{
    Box *b = new Box();

    pa_glib_mainloop *mainloop = pa_glib_mainloop_new(NULL);
    pa_mainloop_api *api = pa_glib_mainloop_get_api(mainloop);
    pa_context *context = pa_context_new(api, "conductor");

    if (pa_context_connect(context, NULL, PA_CONTEXT_NOFAIL, 0) < 0) {
        pa_context_unref(context);
        return;
    }
    pa_context_set_state_callback(context, Box::context_state_cb, static_cast<void*>(b));
}
