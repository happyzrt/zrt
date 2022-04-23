#include <iostream>
#include <vector>
#include "tunnel_factory.h"

int main(int argc, char* argv[])
{
    std::vector<std::string> para;
    for (int i = 1; i < argc; i++) {
        para.emplace_back(std::string(argv[i]));
    }
    tunnel* tun = tunnel_factory::instance().get_tunnel(para);
    if (tun == nullptr) {
        std::cout << "create tunnel failed" << std::endl;
        return 0;
    }
    tun->start_work();
    
    int tmp;
    std::cin >> tmp;
    return 0;
}
