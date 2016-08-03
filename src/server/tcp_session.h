#ifndef TCP_SESSION
#define TCP_SESSION

#include <boost/asio/ip/tcp.hpp>
#include <boost/thread.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/enable_shared_from_this.hpp>

class tcp_session;

namespace toster {
namespace server {

typedef boost::system::error_code error_code;

enum session_state { runnig = 0, stopped, faulted };
struct session_statistics {
    session_statistics() {id = rand() %65535;}
    session_state  state = stopped;
    int id;
    size_t bytes_r = 0;
    size_t bytes_w = 0;
    error_code last_error;
    size_t error_quantity = 0;
};

class tcp_session : public boost::enable_shared_from_this<tcp_session>, boost::noncopyable {

public:
    typedef boost::shared_ptr<tcp_session> ptr;
    ~tcp_session();
    tcp_session(boost::asio::io_service& serv, std::vector<ptr> & arr, boost::recursive_mutex& mut);
    void start();
    void stop();
    bool is_stopped(){return !(sock_.is_open());}
    const session_statistics & statistic(){return stat_;}
    boost::asio::ip::tcp::socket & sock() { return sock_;}

private:
    void stop_del();
    void on_read(const error_code & err, size_t bytes);
    void on_write(const error_code & err, size_t bytes);
    void do_read();
    void do_write(const std::string & msg);

    boost::asio::ip::tcp::socket sock_;
    boost::asio::streambuf read_buffer, write_buffer;
    session_statistics stat_;
    std::vector<tcp_session::ptr> & sess_array_;
    boost::recursive_mutex& serv_mutex;
};





} //server
} //toster

#endif /* TCP_SESSION */
