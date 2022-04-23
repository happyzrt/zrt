#include <iostream>
#include "tunnel_client.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

tunnel_client::tunnel_client(std::string local_ip, std::string dst_ip, int dst_port) : tunnel(local_ip, false)
{
    dst_addr.ip = dst_ip;
    dst_addr.port = dst_port;
}
tunnel_client::~tunnel_client()
{
}

bool tunnel_client::start_work()
{
    bool is_connect_success = start_connect();
    if (is_connect_success) {
        tunnel::start_work();
    }
    if (is_connect_success) {
        // modify route table
        sleep(1);
        std::string cmd("/bin/bash /Users/zrt/zrt/architecture/tunnel/script/client.sh");
        system(cmd.c_str());
    }
    return is_connect_success;
}

void tunnel_client::stop_work()
{
    // modify route table
    std::string cmd("/bin/bash /Users/zrt/zrt/architecture/tunnel/script/client_restore.sh");
    system(cmd.c_str());
    tunnel::stop_work();
}

bool tunnel_client::start_connect()
{
    bool is_connect_success = false;
    int nego_fd = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in dest_address;
    memset(&dest_address, 0, sizeof(dest_address));
    dest_address.sin_family = AF_INET;
    dest_address.sin_port = htons(dst_addr.port);
    inet_aton(dst_addr.ip.c_str(), &dest_address.sin_addr);
    std::cout << "connecting to " << dst_addr.ip << ":" << dst_addr.port << std::endl;

    // send connect request to server
    std::string hello_server = "hello, tunnel server";
    sendto(nego_fd, hello_server.c_str(), hello_server.length(), 0, (struct sockaddr *)&dest_address, sizeof(struct sockaddr_in));

    // get confirm from server
    uint8_t buff[4096];
    fd_set socket_mask;
    struct timeval wait_time;
    wait_time.tv_sec = 7;
    wait_time.tv_usec = 0;
    int max_sock = 0;
    FD_ZERO(&socket_mask);
    FD_SET(nego_fd, &socket_mask);
    if (nego_fd > max_sock) {
        max_sock = nego_fd;
    }

    if (select(max_sock + 1, &socket_mask, nullptr, nullptr, &wait_time) > 0) {
        if(FD_ISSET(nego_fd, &socket_mask)) {
            int size = recvfrom(nego_fd, buff, 4096, 0, nullptr, nullptr);
            if (memcmp(buff, "hello, tunnel client", size) == 0) {
                std::cout << "nego success" << std::endl;
                is_connect_success = true;
            }
        }
    }
    return is_connect_success;
}
