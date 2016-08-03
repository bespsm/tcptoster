#ifndef SERVER_CORE
#define SERVER_CORE

#include <common_tools.h>
#include "tcp_session.h"
#include "boost/asio/signal_set.hpp"

namespace toster {
namespace server {

class server_core : private boost::noncopyable {

public:
    explicit server_core(const server_config &conf);
    explicit server_core();
    void init(const server_config &conf);
    void start();
    void stop();
    void status();
    void kill_sessions();
    void log_file();
    void log_console();
private:
    void log(std::ostream &str);
    void thread_listen();
    void handle_accept(tcp_session* client, const error_code & err);
    //void check_sessions(const error_code & err);
    //void thread_listen_diagn();

    int port;
    size_t thread_count;
    std::vector<tcp_session::ptr> sessions;
    boost::asio::io_service service;
    boost::asio::signal_set signals_;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::recursive_mutex core_mutex;
    bool is_init;
};

} //server
} //toster

#endif /* SERVER_CORE */
