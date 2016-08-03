#ifndef CLIENT_MANAGER
#define CLIENT_MANAGER

#include "common_tools.h"
#include "client_session.h"
#include "boost/asio/signal_set.hpp"
#include <vector>

namespace toster {
namespace client {

class client_manager {
public:
    client_manager(boost::asio::io_service & service);
    void init(const client_config &conf);
    void create_sessions();
    void connect_sessions();
    void delete_sessions();
    void run_test();

    int16_t& get_sessions() {return sessions_count;}
    int16_t& get_data_size() {return data_size;}
    int16_t& get_test_attempts() {return test_attempts;}
    long& get_conn_duration() {return conn_duration;}
    std::string sessions_statistic();

private:
    boost::asio::ip::tcp::endpoint ep_;
    boost::asio::io_service & service_;
    std::vector<client_session::ptr> sessions_;
    std::vector<client_stat> array_statistics;

    int16_t sessions_count;
    int16_t data_size;
    int16_t test_attempts;

    int16_t on_connected_sessions_;
    boost::posix_time::ptime conn_start;
    boost::posix_time::ptime conn_end;
    long conn_duration;
    boost::recursive_mutex manager_mutex;
    boost::asio::io_service::work* work_;
};
} //client
} //toster

#endif /* CLIENT_MANAGER */
