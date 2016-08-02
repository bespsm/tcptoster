#ifndef SERVER_CORE
#define SERVER_CORE


//struct server_config;
#include <common_tools.h>
#include "tcp_session.h"
#include "boost/asio/signal_set.hpp"
#include <boost/asio/io_service.hpp>
#include <vector>


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
    void get_log();
private:
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
