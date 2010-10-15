#include <QtDebug>
#include "moduleloader.h"

void Module::load_module_combine_cb(pa_context *, uint32_t success, void *)
{
    if (success) {
        qDebug() << "load module combine successful";
    } else {
        qWarning() << "load module combine failed";
    }
}

void Module::load_module_tunnel_cb(pa_context *, uint32_t success, void *)
{
    if (success) {
        qDebug() << "load module tunnel successful";
    } else {
        qWarning() << "load module tunnel failed";
    }
}

void Module::set_loaded_tunnel_module(bool flag)
{
    tunnel_flag = flag;
}

void Module::set_loaded_combine_module(bool flag)
{
    combine_flag = flag;
}

void Module::load_module_tunnel(pa_context * c, const char * server )
{
    pa_operation * o;
    if(!combine_flag){
        if(!(o = pa_context_load_module(c, "module-tunnel-sink", server, load_module_tunnel_cb , NULL))){
            qWarning() << "Failed loading module-tunnel-sink";
            return;
        }
        this->set_loaded_tunnel_module(true);
        pa_operation_unref(o);
    }
}

void Module::load_module_combine(pa_context * c, QList<uint32_t> slaves)
{
    if(tunnel_flag and !combine_flag){

        pa_operation * o;

        //std::string s1(to_string(i->index));
        //std::string s2 = std::string("slaves=0,");

        //const char * args = (s2+s1).c_str();

        if(!(o = pa_context_load_module(c, "module-combine", /*args*/"slaves=0,0", load_module_combine_cb , NULL))){
            qWarning() << "Failed loading module-combine ...";
            return;
        }
        this->set_loaded_combine_module(true);
        pa_operation_unref(o);

    }

}
