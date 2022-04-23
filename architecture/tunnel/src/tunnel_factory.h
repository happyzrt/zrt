#ifndef __TUNNEL_FACTORY_H__
#define __TUNNEL_FACTORY_H__
#include "tunnel.h"
#include <vector>

class tunnel_factory {
public:
    static tunnel_factory& instance();
    tunnel* get_tunnel(std::vector<std::string> &para);
private:
    bool para_check(std::vector<std::string> &para, bool &is_server);
};
#endif
