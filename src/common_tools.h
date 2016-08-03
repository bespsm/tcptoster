#ifndef COMMON_TOOLS
#define COMMON_TOOLS
#include <boost/system/error_code.hpp>

namespace toster {


std::string get_date();
std::string get_time();

struct server_config {
    server_config(const std::string &filename);
    size_t threads;
    int port;
};

struct client_config {
    client_config(const std::string &filename);
    std::string address;
    int16_t threads;
    int16_t port;
    int16_t sessions;
    int16_t data_size;
    int16_t test_attempts;
};

} //toster

#endif /* COMMON_TOOLS */
