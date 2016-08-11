#include "client_session.h"
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio/error.hpp>

namespace toster {
namespace client {

int client_stat::counter = 0;    //init counter

client_session::~client_session() {
    //std::cout << "bye session" << std::endl;
}

client_session::client_session(
        boost::asio::io_service &service,
        const int16_t &data_size,
        const int16_t &test_attempts,
        boost::asio::ip::tcp::endpoint& ep):
    ep_(ep),
    sock_(service),
    data_size_(data_size),
    test_attempts_(test_attempts),
    stat_(){
    stat_.is_connected = false;
}

void client_session::init_stat(){
    stat_.echo_time.clear();
    stat_.attempts_fault = 0;
    stat_.attempts_succes = 0;
    stat_.attempts = test_attempts_;
}

void client_session::connect(){
    sock_.async_connect(ep_,[this](const error_code& err){
    if(err){
        boost::recursive_mutex::scoped_lock lk(session_mutex);
        this->stat_.is_connected = false;
        std::cerr << err << '\n';
    }
    else{
        this->stat_.is_connected = true;
    }
    this->stat_.conn_time = boost::posix_time::microsec_clock::universal_time();
    });
}

void client_session::run(){
    if (stat_.is_connected == false)
        std::cerr << "not connected" << '\n';
    else
        init_stat();
        do_write();
}

void client_session::close(){
    error_code err;
    std::string buff = "stop";
    buff += + '\n';
    // synch closing socket
    boost::recursive_mutex::scoped_lock lk(session_mutex);
    sock_.write_some(boost::asio::buffer(buff),err);
    this->sock_.cancel();
    this->sock_.close();
}

void client_session::do_read(){
    boost::asio::async_read(sock_,boost::asio::buffer(read_buffer_,data_size_+1),boost::asio::transfer_at_least(data_size_+1),
    [this](const error_code & err, size_t bytes){
        if (err){
            std::cerr << "error: " << err << '\n';
        }
        this->attempt_end = boost::posix_time::microsec_clock::universal_time();
        this->stat_.echo_time.push_back((this->attempt_end - this->attempt_start).total_milliseconds());
        this->stat_.attempts_succes++;
        for(int16_t i = 0; i<data_size_ ;i++){
            if (this->read_buffer_[i] != this->write_buffer_[i]) {
                this->stat_.attempts_fault++;
                this->stat_.attempts_succes--;
                break;
            }
        }
        this->stat_.attempts--;
        this->do_write();
    });
}

void client_session::do_write(){
    if(this->stat_.attempts > 0){
        srand(time(nullptr));
        for (int16_t i=0;i < data_size_;i++){
            write_buffer_[i] = rand()%64 + 64;
        }
        this->write_buffer_[data_size_] = '\n';
        attempt_start = boost::posix_time::microsec_clock::universal_time();

        sock_.async_write_some(boost::asio::buffer(write_buffer_,data_size_+1),
        [this](const error_code & err, size_t bytes){
            if (err) std::cerr << "error: " << err << '\n';
            this->do_read();
        });
        //finish test
        this->stat_.test_time = boost::posix_time::microsec_clock::universal_time();
    }
}

} //client
} //toster
