#include "n2n.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
void process_buffer(unsigned char* buffer, int rc);
int main(int argc, char* argv[])
{
    int port = 0;
    struct sockaddr_in server_address;
    if(argc ==2)
    {
        port = atoi(argv[1]);
        printf("udp listening on port %d\n", port);
    }
    else
    {
        port = atoi(argv[2]);
        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        inet_aton(argv[1], &server_address.sin_addr);
    }
    tuntap_dev device;
    memset(&device, 0, sizeof(tuntap_dev));
    int tap;
    if(argc == 2)
    {
        tap = tuntap_open(&device, "tap0", "static", "192.168.7.1", "255.255.255.0", "00:0c:29:d4:f8:6e", DEFAULT_MTU);
    }
    else
    {
        tap = tuntap_open(&device, "tap0", "static", "192.168.7.7", "255.255.255.0", "00:0c:29:d4:f8:6d", DEFAULT_MTU);
    }
    if(tap < 0)
    {
        printf("open tap device failed\n");
        return -1;
    }
    
    int sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
    int sockopt = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));
    if(argc == 2)
    {
        struct sockaddr_in local_address;
        memset(&local_address, 0, sizeof(local_address));
        local_address.sin_family = AF_INET;
        local_address.sin_port = htons(port);
        local_address.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(sock_fd,(struct sockaddr*) &local_address, sizeof(local_address));
    }
    
    fd_set socket_mask;
    int max_sock = sock_fd;
    struct timeval wait_time;
    wait_time.tv_sec = 1;
    wait_time.tv_usec = 0;
    struct sockaddr_in client_addr;
    unsigned char buff[4096];
    int rc = 0;
    int length;
    int ready = 0;
    while(1)
    {
        if(argc == 2)
        {
            FD_ZERO(&socket_mask);
            FD_SET(sock_fd, &socket_mask);
            FD_SET(tap, &socket_mask);
            if(tap > max_sock)
            {
                max_sock = tap;
            }
            
            rc = select(max_sock+1, &socket_mask, NULL, NULL, &wait_time);
            if(rc > 0)
            {
                if(FD_ISSET(sock_fd, &socket_mask))
                {
                    memset(buff, 0, 4096);
                    rc = recvfrom(sock_fd, buff, 4096, 0, (struct sockaddr *)&client_addr, &length);
                    if(strstr(buff, "hello"))
                    {
                        printf("ready\n");
                        ready = 1;
                        continue;
                    }
                    if(ready)
                    {
                        process_buffer(buff, rc);
                        //printf("<-------%d\n", rc);
                        tuntap_write(&device, buff, rc);
                    }
                }
                if(FD_ISSET(tap, &socket_mask) && ready)
                {
                    memset(buff, 0, 4096);
                    rc = tuntap_read(&device, buff, 4096);
                    //printf("------->\n");
                    process_buffer(buff, rc);
                    sendto(sock_fd, buff, rc, 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in));
                }
            }
        }
        else
        {
            if(ready == 0)
            {
                sendto(sock_fd, "hello", 6, 0, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
                ready = 1;
                
                continue;
            }
            
            FD_ZERO(&socket_mask);
            FD_SET(sock_fd, &socket_mask);
            FD_SET(tap, &socket_mask);
            if(tap > max_sock)
            {
                max_sock = tap;
            }
            rc = select(max_sock+1, &socket_mask, NULL, NULL, &wait_time);
            if(rc > 0)
            {
                if(FD_ISSET(sock_fd, &socket_mask))
                {
                    memset(buff, 0, 4096);
                    rc = recvfrom(sock_fd, buff, 4096, 0, (struct sockaddr *)&client_addr, &length);
                    //printf("<-------\n");
                    process_buffer(buff, rc);
                    tuntap_write(&device, buff, rc);
                }
                if(FD_ISSET(tap, &socket_mask))
                {
                    memset(buff, 0, 4096);
                    rc = tuntap_read(&device, buff, 4096);
                    //printf("%d------->\n", rc);
                    process_buffer(buff, rc);
                    sendto(sock_fd, buff, rc, 0, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
                }
            }
            
        }
    }
    
    tuntap_close(&device);
    return 0;
}
void process_buffer(unsigned char* buff, int rc)
{
    int i = 0;
    for(i=0; i<rc; i++)
    {
        buff[i] = ~buff[i];
    }
}
