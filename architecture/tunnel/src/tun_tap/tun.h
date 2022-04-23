#ifndef __TUN_TAP_H__
#define __TUN_TAP_H__
#include "stdint.h"
#include <string>

class tun {
public:
    tun(std::string dev_ip, std::string mask, std::string gateway, int mtu);
    ~tun();
    int open();
    int read(uint8_t* buff, size_t buff_size);
    void write(uint8_t* buff, size_t buff_size);

private:
    std::string tun_ip;
    std::string tun_mask;
    std::string tun_gateway;
    int tun_mtu;
    int tun_fd;
    std::string tun_name;
};
#endif
