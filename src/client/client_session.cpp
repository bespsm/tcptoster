#include "client_session.h"
#include <boost/bind.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <iostream>

namespace toster {
namespace client {

client_session::~client_session() {
    std::cout << "bye session" << std::endl;
}

client_session::client_session(boost::asio::io_service &service):
    sock_(service),
    state_(init){}

void client_session::set_conn_success(){
    //boost::recursive_mutex::scoped_lock lk(session_mutex);
    state_ = session_state::conn_success;
}

void client_session::set_conn_error(){
    //boost::recursive_mutex::scoped_lock lk(session_mutex);
    state_ = session_state::conn_error;
}

} //client
} //toster
