#ifndef CLIENT_CORE
#define CLIENT_CORE

#include "client_manager.h"
#include <boost/thread.hpp>
#include <common_tools.h>
#include "boost/asio/signal_set.hpp"
#include <boost/asio/io_service.hpp>
#include "boost/date_time/posix_time/posix_time_types.hpp"

namespace toster {
namespace client {

class client_core : private boost::noncopyable {

public:
    explicit client_core(const client_config &conf);
    explicit client_core();
    void init(const client_config &conf);
    void connect();
    void disconnect();
    bool is_run(){return !service.stopped();}
    void run_test();
    //void get_log();
    void close();
private:
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
