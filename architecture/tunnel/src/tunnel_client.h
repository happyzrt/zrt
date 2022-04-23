#ifndef __TUNNEL_CLIENT_H__
#define __TUNNEL_CLIENT_H__
#include "tunnel.h"

class tunnel_client : public tunnel {
public:
    tunnel_client(std::string local_ip, std::string dst_ip, int dst_port);
    ~tunnel_client();
    virtual bool start_work() override;
private:
    bool start_connect();
};
#endif
