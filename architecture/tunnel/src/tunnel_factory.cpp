#include "tunnel_factory.h"
#include "tunnel_server.h"
#include "tunnel_client.h"

tunnel_factory& tunnel_factory::instance()
{
    static tunnel_factory factory;
    return factory;
}

tunnel* tunnel_factory::get_tunnel(std::vector<std::string> &para)
{
    tunnel* tun = nullptr;
    bool is_server;
    if (!para_check(para, is_server)) {
        return tun;
    }

    if (is_server) {
        tun = new tunnel_server(para[0], std::stoi(para[1]));
    } else {
        tun = new tunnel_client(para[0], para[1], std::stoi(para[2]));
    }
    return tun;
}

bool tunnel_factory::para_check(std::vector<std::string> &para, bool &is_server)
{
    bool result = true;
    auto para_size = para.size();
    if (para_size > 3 || para_size < 2) {
        result = false;
    }

    if (para_size == 2) {
        is_server = true;
    } else {
        is_server = false;
    }
    return result;
}
