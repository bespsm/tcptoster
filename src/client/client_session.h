#ifndef CLIENT_SESSION
#define CLIENT_SESSION

#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/recursive_mutex.hpp>

class client_session;

namespace toster {
namespace client {

struct client_stat{
    client_stat() {id = rand() %65535;}
    int id;
    std::vector<long> echo_time;
    bool is_connected;
    int16_t attempts_fault;
    int16_t attempts_succes;
    int16_t attempts;
    boost::posix_time::ptime conn_time;
    boost::posix_time::ptime test_time;
};

typedef boost::system::error_code error_code;

class client_session :
    boost::noncopyable {

public:
    typedef boost::shared_ptr<client_session> ptr;
    ~client_session();
    client_session(boost::asio::io_service & service,
        const int16_t& data_size,
        const int16_t& test_attempts,
        boost::asio::ip::tcp::endpoint& ep);

    boost::asio::ip::tcp::socket & sock() { return sock_;}
    client_stat& statistic(){return stat_;}
    void connect();
    void run();
    void close();
private:
    void init_stat();
    void do_write();
    void do_read();
    boost::asio::ip::tcp::endpoint ep_;
    boost::posix_time::ptime attempt_start;
    boost::posix_time::ptime attempt_end;
    boost::asio::ip::tcp::socket sock_;
    int16_t data_size_;
    int16_t test_attempts_;
    client_stat stat_;
    enum { max_msg = 1024 };
    char read_buffer_[max_msg],write_buffer_[max_msg];
    boost::recursive_mutex session_mutex;
};


} //client
} //toster

#endif /* CLIENT_SESSION */
