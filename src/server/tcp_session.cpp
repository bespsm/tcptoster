#include "tcp_session.h"
#include <boost/bind.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <iostream>
#include <algorithm>
#include <boost/thread.hpp>

namespace toster {
namespace server {

tcp_session::~tcp_session() {
    std::cout << "bye session" << std::endl;
}

tcp_session::tcp_session(boost::asio::io_service& serv, std::vector<ptr> & arr, boost::recursive_mutex& mut):
    sock_(serv),
    stat_(),
    sess_array_(arr),
    serv_mutex(mut){}

void tcp_session::start() {
    stat_.state = runnig;
    do_read();
}

void tcp_session::stop_del() {
    if (stat_.state != runnig) return;
    stat_.state = stopped;
    sock_.cancel();
    sock_.close();
    boost::recursive_mutex::scoped_lock lk(serv_mutex);
    sess_array_.erase(
        std::find(sess_array_.begin(),
                  sess_array_.end(),
                  shared_from_this()));
}

void tcp_session::stop() {
    if (stat_.state != runnig) return;
    stat_.state = stopped;
    sock_.cancel();
    sock_.close();
}

void tcp_session::on_read(const error_code & err, size_t bytes) {
    if (!err) {
        std::istream s(&read_buffer);
        std::string line;
        std::getline(s, line);
        if (line.find("stop") < bytes)
            stop_del();
        else {
            stat_.state = runnig;
            stat_.bytes_r += bytes;
            do_write(line + "\n");
        }
    }
    else {
        stat_.error_quantity++;
        stat_.last_error = err;
        stat_.state = faulted;
        do_read();
    }
}

void tcp_session::on_write(const error_code & err, size_t bytes) {
    if (!err) {
        stat_.bytes_w += bytes;
        do_read();
    }
    else {
        stat_.error_quantity++;
        stat_.last_error = err;
        stat_.state = faulted;
        do_read();
    }
}

void tcp_session::do_read() {
   async_read_until(
       sock_, read_buffer, '\n' ,
       boost::bind(&tcp_session::on_read,this,_1,_2));
}

void tcp_session::do_write(const std::string & msg) {
    std::ostream s(&write_buffer);
    s << msg;
    async_write(
        sock_,write_buffer,
        boost::bind(&tcp_session::on_write,this,_1,_2));
}

} //server
} //toster
