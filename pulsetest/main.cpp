#include <QtDebug>
#include <QCoreApplication>

#include <pulse/context.h>
#include <pulse/glib-mainloop.h>
#include <pulse/introspect.h>
#include <pulse/operation.h>

#include <strings.h>


static void server_cb(pa_context *, const pa_server_info *i, void *)
{
	qDebug() << "running @" << i->host_name << '\n';
}

static void move_cb(pa_context *, int success, void *)
{
	if (success) {
		qDebug() << "move successful";
	} else {
		qWarning() << "move failed";
	}
}

static void sink_cb(pa_context *, const pa_sink_info *i, int eol, void *)
{
	if (eol < 0) {
		qWarning() << "sink callback failure\n";
		return;
	} else if (eol > 0) {
		qDebug() << "sink callback done\n";
		return;
	}

	qDebug() << "sink #" << i->index << ":" << i->name;
}

static void sink_input_cb(pa_context *c, const pa_sink_input_info *i, int eol, void *)
{
	if (eol < 0) {
		qWarning() << "sink input callback failure\n";
		return;
	} else if (eol > 0) {
		qDebug() << "sink input callback done\n";
		return;
	}

	qDebug() << "sink input #" << i->index << ":" << i->name;

	if (strcasecmp(i->name, "audio stream") == 0) {
		qDebug() << "moving sink input #" << i->index << "to sink #1 ...";
		pa_operation *o;
		if (!(o = pa_context_move_sink_input_by_index(c, i->index, 1, move_cb, NULL))) {
			qWarning() << "pa_context_move_sink_input_by_index() failed";
			return;
		}
		pa_operation_unref(o);
	}
}

static void source_cb(pa_context *, const pa_source_info *i, int eol, void *)
{
	if (eol < 0) {
		qWarning() << "source callback failure\n";
		return;
	} else if (eol > 0) {
		qDebug() << "source callback done\n";
		return;
	}

	qDebug() << "source #" << i->index << ":" << i->name;
}

static void source_output_cb(pa_context *, const pa_source_output_info *i, int eol, void *)
{
	if (eol < 0) {
		qWarning() << "source output callback failure\n";
		return;
	} else if (eol > 0) {
		qDebug() << "source output callback done\n";
		return;
	}

	qDebug() << "source output #" << i->index << ":" << i->name;
}

static void client_cb(pa_context *, const pa_client_info *i, int eol, void *)
{
	if (eol < 0) {
		qWarning() << "client callback failure\n";
		return;
	} else if (eol > 0) {
		qDebug() << "client callback done\n";
		return;
	}

	qDebug() << "client #" << i->index << ":" << i->name;
}

static void context_state_cb(pa_context *c, void *)
{
	pa_context_state_t state = pa_context_get_state(c);

	if (state == PA_CONTEXT_READY) {
		int local = pa_context_is_local(c);
		qDebug() << "connected to" << (local ? "local" : "remote")
			<< "PulseAudio server" << pa_context_get_server(c);
		pa_operation *o;

		/* SERVER */
		if (!(o = pa_context_get_server_info(c, server_cb, NULL))) {
			qWarning() << "pa_context_get_server_info() failed";
			return;
		}
		pa_operation_unref(o);

		/* SINK */
		if (!(o = pa_context_get_sink_info_list(c, sink_cb, NULL))) {
			qWarning() << "pa_context_get_sink_info_list() failed";
			return;
		}
		pa_operation_unref(o);

		/* SINK INPUT */
		if (!(o = pa_context_get_sink_input_info_list(c, sink_input_cb, NULL))) {
			qWarning() << "pa_context_get_sink_input_info_list() failed";
			return;
		}
		pa_operation_unref(o);

		/* SOURCE */
		/*if (!(o = pa_context_get_source_info_list(c, source_cb, NULL))) {
			qWarning() << "pa_context_get_source_info_list() failed";
			return;
		}
		pa_operation_unref(o);*/

		/* SOURCE OUTPUT */
		/*if (!(o = pa_context_get_source_output_info_list(c, source_output_cb, NULL))) {
			qWarning() << "pa_context_get_source_output_info_list() failed";
			return;
		}
		pa_operation_unref(o);*/

		/* CLIENT */
		if (!(o = pa_context_get_client_info_list(c, client_cb, NULL))) {
			qWarning() << "pa_context_get_client_info_list() failed";
			return;
		}
		pa_operation_unref(o);

	} else if (!PA_CONTEXT_IS_GOOD(state)) {
		qWarning() << "context PHAIL";
		pa_context_unref(c);
		qApp->quit();
	}
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);

	pa_glib_mainloop *mainloop = pa_glib_mainloop_new(NULL);
	pa_mainloop_api *api = pa_glib_mainloop_get_api(mainloop);
	pa_context *context = pa_context_new(api, "pulsetest");

	if (pa_context_connect(context, NULL, PA_CONTEXT_NOFAIL, 0) < 0) {
		pa_context_unref(context);
		return -1;
	}
	pa_context_set_state_callback(context, context_state_cb, NULL);

	return app.exec();
}
