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

void tunnel_client::start_work()
{
    bool is_connect_success = start_connect();
    if (is_connect_success) {
        tunnel::start_work();
    }
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
    int size = recvfrom(nego_fd, buff, 4096, 0, nullptr, nullptr);
    if (memcmp(buff, "hello, tunnel client", size) == 0) {
        std::cout << "nego success" << std::endl;
        is_connect_success = true;
    }
    return is_connect_success;
}
