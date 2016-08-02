#ifndef CLIENT_SESSION
#define CLIENT_SESSION

#include <boost/asio/ip/tcp.hpp>
#include <boost/thread.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/generic/stream_protocol.hpp>
#include <boost/enable_shared_from_this.hpp>

class client_session;

namespace toster {
namespace client {

typedef boost::system::error_code error_code;

class client_session :
    public boost::enable_shared_from_this<client_session>,
    boost::noncopyable {

public:
    enum session_state {conn_success = 0, conn_error, test_success, test_error, init};
    typedef boost::shared_ptr<client_session> ptr;
    ~client_session();
    client_session(boost::asio::io_service & service);

    boost::asio::ip::tcp::socket & sock() { return sock_;}
    const session_state& state(){return state_;}
    void set_conn_success();
    void set_conn_error();
private:
    boost::asio::ip::tcp::socket sock_;
    session_state state_;
    boost::asio::streambuf read_buffer, write_buffer;
    boost::recursive_mutex session_mutex;
};


} //client
} //toster

#endif /* CLIENT_SESSION */
