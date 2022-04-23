#ifndef __TUNNEL_SERVER_H__
#define __TUNNEL_SERVER_H__
#include "tunnel.h"

class tunnel_server : public tunnel {
public:
    tunnel_server(std::string local_ip, uint16_t port);
    virtual bool start_work() override;
private:
    static void listener(tunnel_server* server);
    void accept_connect();
private:
    uint16_t serve_port;
    int listener_id;
};
#endif
