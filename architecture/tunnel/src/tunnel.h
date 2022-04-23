#ifndef __TUNNEL_H__
#define __TUNNEL_H__
#include <string>
#include <mutex>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
class socket_addr {
public:
    std::string ip;
    uint16_t port;
};
class tunnel {
public:
    tunnel(std::string local_ipi, bool is_server);
    ~tunnel();
    virtual void start_work();
    virtual void stop_work();
protected:
    socket_addr dst_addr;
    struct sockaddr_in dest_address;
private:
    int socket_id;
    static void core(tunnel * tun);
    bool tunnel_working = false;
    std::mutex lock;
    std::string tun_tap_ip;
    std::string gateway;
    bool is_server;
};
#endif
