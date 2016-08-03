#ifndef CLIENT_CORE
#define CLIENT_CORE

#include "client_manager.h"
#include "boost/asio/signal_set.hpp"

namespace toster {
namespace client {

class client_core : private boost::noncopyable {

public:
    explicit client_core(const client_config &conf);
    explicit client_core();
    void init(const client_config &conf);
    void start_client();
    void connect();
    void disconnect();
    void run_test();
    void log_file();
    void log_console();
    void close();
private:
    void log(std::ostream &str);
    void thread_listen();

    int16_t threads_count;
    boost::asio::io_service service;
    boost::asio::signal_set signals_;

    boost::recursive_mutex core_mutex;
    client_manager manager_;
    bool is_init;
};

} //client
} //toster

#endif /* CLIENT_CORE */
