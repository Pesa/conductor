#include "SinkInputViewer.h"

void SinkInput::move_cb(pa_context *, int success, void *)
{
	if (success) {
		qDebug() << "move successful";
	} else {
		qWarning() << "move failed";
	}
}

QMap<uint32_t, const char*> SinkInput::get_sinks_input()
{
	return *sinks_input;
}

void SinkInput::sink_input_cb(pa_context *c, const pa_sink_input_info *i, int eol, void * list)
{
	if (eol < 0) {
		qWarning() << "sink input callback failure\n";
		return;
	} else if (eol > 0) {
		qDebug() << "sink input callback done\n";
		return;
	}
	
	QMap< uint32_t, const char* >* sinks_input = static_cast<QMap<uint32_t, const char*>* > (list);
	sinks_input->insert(i->index, i->name);
	
	list = sinks_input;
	
	qDebug() << "sink input #" << i->index << ":" << i->name;
}


void SinkInput::determine_sink_input_available(pa_context *c)
{
	pa_operation * o;
	
	if (!(o = pa_context_get_sink_input_info_list(c, sink_input_cb, static_cast<void*>(sinks_input)))) {
			qWarning() << "pa_context_get_sink_input_info_list() failed";
			return;
		}
		pa_operation_unref(o);
}

void SinkInput::move_audio_stream(pa_context* c, uint32_t new_sink)
{
	QMap<uint32_t, const char*>::iterator it;
	for ( it = sinks_input->begin(); it != sinks_input->end(); ++it ) {

		if (strcasecmp(it.value(), "audio stream") == 0) {
			qDebug() << "moving sink input #" << it.key() << "to sink #" << new_sink << "...";
			pa_operation *o;
			if (!(o = pa_context_move_sink_input_by_index(c, it.key(), new_sink, move_cb, NULL))) {
				qWarning() << "pa_context_move_sink_input_by_index() failed";
				return;
			}			
			
			pa_operation_unref(o);
		}
	}	
}
