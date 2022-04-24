#include <iostream>
#include "tunnel_server.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>

tunnel_server::tunnel_server(std::string local_ip, uint16_t port) : tunnel(local_ip, true) , serve_port(port)
{
    listener_id = socket(PF_INET, SOCK_DGRAM, 0);
    if (listener_id < 0) {
        std::cout << "open listening socket faild" << std::endl;
    }
    struct sockaddr_in local_address;
    memset(&local_address, 0, sizeof(local_address));
    local_address.sin_family = AF_INET;
    local_address.sin_port = htons(serve_port);
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(listener_id,(struct sockaddr*) &local_address, sizeof(local_address));
    if (ret < 0) {
        std::cout << "bind port faild" << std::endl;
    }
}

void tunnel_server::listener(tunnel_server* server)
{
    fd_set socket_mask;
    struct timeval wait_time;
    wait_time.tv_sec = 7;
    wait_time.tv_usec = 0;
    int max_sock = 0;
    while(true) {
        FD_ZERO(&socket_mask);
        FD_SET(server->listener_id, &socket_mask);
        if(server->listener_id > max_sock){
            max_sock = server->listener_id;
        }

        if (select(max_sock+1, &socket_mask, nullptr, nullptr, &wait_time) > 0) {
            if(FD_ISSET(server->listener_id, &socket_mask)) {
                server->accept_connect();
            }
        } else {
            sleep(1);
        }
    }
}

bool tunnel_server::start_work()
{
    std::thread server(listener, this);
    server.detach();
    return true;
}

void tunnel_server::accept_connect()
{
    uint8_t buff[4096];
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(struct sockaddr_in);

    // get client connect request
    auto size = recvfrom(listener_id, buff, 4096, 0, (struct sockaddr *)&client_addr, &length);
    if (memcmp(buff, "hello, tunnel server", size) != 0) {
        std::cout << "bad request:" << std::string((char*)buff) << std::endl;
        return;
    }

    // stop current connection
    tunnel::stop_work();

    // start new connection
    if (!tunnel::start_work()) {
        tunnel::stop_work();
        return;
    }

    // send reply to client
    std::string hello_client = "hello, tunnel client";
    sendto(listener_id, hello_client.c_str(), hello_client.length(), 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in));
    dst_addr.ip = inet_ntoa(client_addr.sin_addr);
    std::cout << "nego success: " << dst_addr.ip << std::endl;

    // open server router function
    std::string cmd("/usr/bin/bash /zrt/architecture/tunnel/script/server.sh ");
    system(cmd.c_str());
}
