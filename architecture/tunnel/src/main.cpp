#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include "tunnel_factory.h"

tunnel* tun = nullptr;
void sig_handle(int num)
{
    if (tun != nullptr) {
        tun->stop_work();
        tun = nullptr;
    }
}
int main(int argc, char* argv[])
{
    std::vector<std::string> para;
    for (int i = 1; i < argc; i++) {
        para.emplace_back(std::string(argv[i]));
    }
    tun = tunnel_factory::instance().get_tunnel(para);
    if (tun == nullptr) {
        std::cout << "create tunnel failed" << std::endl;
        return 0;
    }

    tun->start_work();
    struct sigaction newact;
    newact.sa_handler = sig_handle;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGINT, &newact, nullptr);

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    int num;
    sigwait(&set, &num);
    std::cout << num << std::endl;
    return 0;
}
