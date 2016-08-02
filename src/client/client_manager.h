#ifndef CLIENT_MANAGER
#define CLIENT_MANAGER

#include "common_tools.h"
#include "client_session.h"
#include "boost/asio/signal_set.hpp"
//#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <vector>

namespace toster {
namespace client {

class client_manager {
public:
    client_manager(boost::asio::io_service & service);
    void create_sessions();
    void init(const client_config &conf);
    void connect_sessions();
    void disconnect_sessions();
    void stop();

private:
    void asyn_conn(client_session::ptr &n);

    boost::asio::ip::tcp::endpoint ep_;
    boost::asio::io_service & service_;
    std::vector<client_session::ptr> vector_;

    int16_t sessions_count;
    int16_t data_size;
    int16_t data_queue;

    int16_t on_connected_sessions_;
    boost::posix_time::ptime conn_start;
    boost::posix_time::ptime conn_end;
    long long conn_duration;
    boost::recursive_mutex manager_mutex;
    boost::asio::io_service::work* work_;
};
} //client
} //toster

#endif /* CLIENT_MANAGER */
