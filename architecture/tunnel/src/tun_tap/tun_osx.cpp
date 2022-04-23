#include "tun.h"
#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <net/if_utun.h>
#include <sys/kern_control.h>
#include <sys/sys_domain.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
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
    int utun_id;
    for (int utun_id = 0; utun_id < 256; ++utun_id) {
        struct sockaddr_ctl sc;
        struct ctl_info control_info;

        memset(&control_info, 0, sizeof(control_info));
        std::string utun_conrol_name(UTUN_CONTROL_NAME);
        memcpy(control_info.ctl_name, utun_conrol_name.c_str(), utun_conrol_name.length());

        tun_fd = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
        if (ioctl(tun_fd, CTLIOCGINFO, &control_info) == -1) {
            std::cout << "ioctl failed" << std::endl;
            return -1;
        }

        //int var = true;;
        //if (setsockopt(tun_fd, SYSPROTO_CONTROL, UTUN_OPT_ENABLE_NETIF, &var, sizeof(var)) != 0) {
        //    perror("UTUN_OPT_ENABLE_NETIF");
        //    std::cout << "setsockopt failed" << std::endl;
        //    return -1;
        //}

        sc.sc_id = control_info.ctl_id;
        sc.sc_len = sizeof(sc);
        sc.sc_family = AF_SYSTEM;
        sc.ss_sysaddr = AF_SYS_CONTROL;
        sc.sc_unit = utun_id;
        memset(sc.sc_reserved, 0, sizeof(sc.sc_reserved));

        // a utun device will be created, when the following connect success
        if (connect(tun_fd, (struct sockaddr *)&sc, sizeof(sc)) == -1) {
            continue;
        }

        // get utun device name
        char utunname[20];
        socklen_t utunname_len = sizeof(utunname);
        if (getsockopt(tun_fd, SYSPROTO_CONTROL, UTUN_OPT_IFNAME, utunname, &utunname_len) == 0) {
            tun_name = std::string(utunname);
        }
        std::cout << tun_name << std::endl;

        // set utun attr
        std::string cmd("/sbin/ifconfig ");
        cmd = cmd + tun_name + " " + tun_ip + " " + tun_gateway + " netmask " + tun_mask + " mtu " + std::to_string(tun_mtu) + " -arp up";
        system(cmd.c_str());
        break;
    }
    return utun_id < 256 ? tun_fd : -1;
}

int tun::read(uint8_t* buff, size_t buff_size)
{
    return ::read(tun_fd, buff, buff_size);
}

void tun::write(uint8_t* buff, size_t buff_size)
{
    ::write(tun_fd, buff, buff_size);
}
