#include "tunnel.h"
#include <thread>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include "tun.h"

namespace {
void process_buff(uint8_t* buff, int size)
{
    uint8_t tmp;
    for (int i = 0; i < size; i++) {
        buff[i] = ~buff[i];
    }
}
}

tunnel::tunnel(std::string local_ip, bool is_server)
{
    tun_tap_ip = local_ip;
    gateway = "192.168.13.1";
    this->is_server = is_server;
}

tunnel::~tunnel()
{
}

void tunnel::core(tunnel* tun)
{
    std::lock_guard<std::mutex> lck(tun->lock);
    // open tun
    class tun tun_tap(tun->tun_tap_ip, "255.255.255.0", tun->gateway, 1300);
    int tun_tap_id = tun_tap.open();
    if (tun_tap_id < 0) {
        std::cout << "open tun_tap failed" << std::endl;
        return;
    }

    std:: cout << "tunnel ready" << std::endl;

    fd_set socket_mask;
    struct timeval wait_time;
    wait_time.tv_sec = 1;
    wait_time.tv_usec = 0;
    int max_sock = tun->socket_id;
    bool ready = false;
    if (tun->is_server) {
        uint8_t buff[4096];
        socklen_t length = sizeof(struct sockaddr_in);

        // get client connect request
        FD_ZERO(&socket_mask);
        FD_SET(tun->socket_id, &socket_mask);
        if (select(max_sock+1, &socket_mask, nullptr, nullptr, &wait_time) > 0) {
            if(FD_ISSET(tun->socket_id, &socket_mask)) {
                auto size = recvfrom(tun->socket_id, buff, 4096, 0, (struct sockaddr *)&tun->dest_address, &length);
                if (memcmp(buff, "hello, tunnel server", size) != 0) {
                    std::cout << "bad request:" << std::string((char*)buff) << std::endl;
                }
                ready = true;
            }
        }
        if (!ready) {
            return;
        }

        // send reply to client
        std::string hello_client = "hello, tunnel client";
        sendto(tun->socket_id, hello_client.c_str(), hello_client.length(), 0, (struct sockaddr *)&tun->dest_address, sizeof(struct sockaddr_in));
        std::cout << "tunnel server ready" << std::endl;
    } else {
        // send connect request to server
        std::string hello_server = "hello, tunnel server";
        sendto(tun->socket_id, hello_server.c_str(), hello_server.length(), 0, (struct sockaddr *)&tun->dest_address, sizeof(struct sockaddr_in));

        // get confirm from server
        uint8_t buff[4096];
        FD_ZERO(&socket_mask);
        FD_SET(tun->socket_id, &socket_mask);
        if (select(max_sock+1, &socket_mask, nullptr, nullptr, &wait_time) > 0) {
            if(FD_ISSET(tun->socket_id, &socket_mask)) {
                int size = recvfrom(tun->socket_id, buff, 4096, 0, nullptr, nullptr);
                if (memcmp(buff, "hello, tunnel client", size) == 0) {
                    std::cout << "tunnel client ready" << std::endl;
                    ready = true;
                }
            }
        }
        if (!ready) {
            return;
        }
     }

    uint8_t buff[4096];
    int size;
    max_sock = tun_tap_id;
    while(tun->tunnel_working) {
        FD_ZERO(&socket_mask);
        FD_SET(tun->socket_id, &socket_mask);
        FD_SET(tun_tap_id, &socket_mask);
        if(tun->socket_id > max_sock){   
            max_sock = tun->socket_id;
        }   
    
        if (select(max_sock+1, &socket_mask, nullptr, nullptr, &wait_time) > 0) {
           if(FD_ISSET(tun->socket_id, &socket_mask)) {
               size = recvfrom(tun->socket_id, buff, 4096, 0, nullptr, nullptr);
               process_buff(buff, size);
               #ifdef OSX_SYSTEM
               static uint8_t head_buffer[4096] = {0, 0, 0, 2};
               memcpy(head_buffer + 4, buff, size);
               tun_tap.write(head_buffer, size + 4);
               #else
               tun_tap.write(buff, size);
               #endif
           } 
           if(FD_ISSET(tun_tap_id, &socket_mask)) {
               size = tun_tap.read(buff, 4096);
               #ifdef OSX_SYSTEM
               // on osx system, utun device is a loopback device on which there is a fixed head of "02 00 00 00"
               if (size <= 4) {
                   continue;
               }
               process_buff(buff + 4, size - 4);
               sendto(tun->socket_id, buff + 4, size - 4, 0, (struct sockaddr *)&tun->dest_address, sizeof(struct sockaddr_in));
               #else
               process_buff(buff, size);
               sendto(tun->socket_id, buff, size, 0, (struct sockaddr *)&tun->dest_address, sizeof(struct sockaddr_in));
               #endif
           } 
        } else {
            usleep(1000);
        }
    }
}

bool tunnel::start_work()
{
    tunnel_working = true;
    socket_id = socket(PF_INET, SOCK_DGRAM, 0);
    if (socket_id < 0) {
        std::cout << "open udp socket fail" << std::endl;
    }
    if (is_server) {
        // server side bind local port
        struct sockaddr_in local_address;
        memset(&local_address, 0, sizeof(local_address));
        local_address.sin_family = AF_INET;
        local_address.sin_port = htons(5858);
        local_address.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(socket_id, (struct sockaddr*) &local_address, sizeof(local_address));
    } else {
        //client side prepare dest addr
        memset(&dest_address, 0, sizeof(dest_address));
        dest_address.sin_family = AF_INET;
        dest_address.sin_port = htons(5858);
        inet_aton(dst_addr.ip.c_str(), &dest_address.sin_addr);
    }
    std::thread worker(core, this);
    worker.detach();
    return true;
}

void tunnel::stop_work()
{
    tunnel_working = false;
    std::lock_guard<std::mutex> lck(lock);
    if (socket_id > 0) {
        close(socket_id);
    }
}
