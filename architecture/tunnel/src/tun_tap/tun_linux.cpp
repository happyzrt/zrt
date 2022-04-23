#include "tun.h"
#include <linux/if_tun.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "stdint.h"
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <string>

tun::tun(std::string dev_ip, std::string mask, std::string gateway, int mtu)
{
    tun_ip = dev_ip;
    tun_mask = mask;
    tun_gateway = gateway;
    tun_mtu = mtu;
}
tun::~tun()
{
    close(tun_fd);
}

int tun::open()
{
    tun_fd = ::open("/dev/net/tun", O_RDWR);
    if (tun_fd < 0) {
        std::cout << "open tun device failed" << std::endl;
        return -1;
    }

    // create a tun device
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN|IFF_NO_PI;
    tun_name = std::string("tun0");
    memcpy(ifr.ifr_name, tun_name.c_str(), tun_name.length());
    if (ioctl(tun_fd, TUNSETIFF, (void *)&ifr) < 0) {
        std::cout << "ioctl failed" << std::endl;
        return -1;
    }

    // set tun attr : ip/gateway/netmask/mtu
    memset(&ifr, 0, sizeof(ifr));
    memcpy(ifr.ifr_name, tun_name.c_str(), tun_name.length());
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (fd < 0) {
        std::cout << "get ioctl fd failed" << std::endl;
        return -1;
    }

    // ip
    ifr.ifr_addr.sa_family = AF_INET;
    inet_pton(AF_INET, tun_ip.c_str(), &((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr);
    if (ioctl(fd, SIOCSIFADDR, &ifr) == -1) {
        std::cout << "set tun ip failed" << std::endl; 
        return -1;
    }

    // gateway
    inet_pton(AF_INET, tun_gateway.c_str(), &((struct sockaddr_in*)&ifr.ifr_dstaddr)->sin_addr);
    if (ioctl(fd, SIOCSIFDSTADDR, &ifr) == -1) {
        std::cout << "set tun ip failed" << std::endl; 
        return -1;
    }

    // mask
    inet_pton(AF_INET, tun_mask.c_str(), &((struct sockaddr_in*)&ifr.ifr_netmask)->sin_addr);
    if(ioctl(fd, SIOCSIFNETMASK, &ifr) == -1) {
        std::cout << "set tun mask failed" << tun_mask << std::endl; 
        return -1;
    }

    // mtu
    ifr.ifr_mtu = tun_mtu;
    if(ioctl(fd, SIOCSIFMTU, &ifr) == -1) {
        std::cout << "set tun mtu failed" << std::endl; 
        return -4;
    }

    // up
    if(ioctl(fd, SIOCGIFFLAGS, &ifr) == -1) {
        std::cout << "set tun mtu failed" << std::endl; 
        return -1;
    }
    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
    if(ioctl(fd, SIOCSIFFLAGS, &ifr) == -1) {
        std::cout << "set tun mtu failed" << std::endl; 
        return -1;
    }

    return tun_fd;
}

int tun::read(uint8_t* buff, size_t buff_size)
{
    return ::read(tun_fd, buff, buff_size);
}

void tun::write(uint8_t* buff, size_t buff_size)
{
    ::write(tun_fd, buff, buff_size);
}
