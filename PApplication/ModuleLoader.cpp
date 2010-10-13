#include "ModuleLoader.h"


void Module::load_module_cb(pa_context *, uint32_t success, void *)
{
	if (success) {
		qDebug() << "load successful";
	} else {
		qWarning() << "load failed";
	}
}

void Module::set_loaded_tunnel_module(bool flag){tunnel_flag = flag;}

void Module::set_loaded_combine_module(bool flag){combine_flag = flag;}

void Module::load_module_tunnel(pa_context * c, pa_operation * o, QString server )
{
	//qDebug() << "Loading module-tunnel-sink #" << i->index << " ...";
	
		if(!(o = pa_context_load_module(c, "module-tunnel-sink", "server=127.0.0.1", load_module_cb , NULL))){
			qWarning() << "Failed loading module-tunnel-sink";
			return;
		}
		this->set_loaded_tunnel_module(true);
		pa_operation_unref(o);
	
}





void Module::load_module_combine(pa_context * c, pa_operation * o, QList<uint32_t> slaves)
{
	/*if(i->index != 0) {
	qDebug() << "Loading module-combine slaves #0 and #" << i->index << " ...";
	
	pa_operation * o;
	
	std::string s1(to_string(i->index));
	std::string s2 = std::string("slaves=0,");
	
	const char * args = (s2+s1).c_str();
 
		if(!(o = pa_context_load_module(c, "module-combine", args, load_module_cb , NULL))){
			qWarning() << "Failed loading module-combine ...";
			return;
		}
		pa_operation_unref(o);
	}
*/
}
