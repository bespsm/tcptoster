#include "client_manager.h"

namespace toster {
namespace client {

client_manager::client_manager(
        boost::asio::io_service &service):
        service_(service){
}

void client_manager::init(const toster::client_config &conf){
    ep_ = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(conf.address), int(conf.port));
    sessions_count = conf.sessions;
    data_size = conf.data_size;
    data_queue = conf.data_queue;
}

void client_manager::create_sessions(){
    for (int16_t i = 0; i < sessions_count; ++i){
        client_session::ptr shared_sess(new client_session(service_));
        vector_.push_back(shared_sess);
    }
}

void client_manager::connect_sessions() {
    for(auto a = vector_.begin();a != vector_.end();a++) {
        if (a == vector_.begin())
                conn_start = boost::posix_time::microsec_clock::universal_time();
        (*a)->sock().async_connect(ep_,[&,a](const error_code& err){
            if(err){
                (*a)->set_conn_error();
                std::cerr << "error:" << err.message() << '\n';
            }
            else{
                (*a)->set_conn_success();
                std::cout << "succses conn: " << boost::this_thread::get_id() << '\n';
            }
            boost::recursive_mutex::scoped_lock lk(this->manager_mutex);
            if (++on_connected_sessions_ == sessions_count){
                conn_end = boost::posix_time::microsec_clock::universal_time();
                conn_duration = (conn_end - conn_start).total_milliseconds();
                work_ = new boost::asio::io_service::work(service_);
                std::cout << "duration: " << this->conn_duration << '\n';
            }
        });
    }
}


void client_manager::disconnect_sessions()
{
    boost::recursive_mutex::scoped_lock lk(manager_mutex);
    if(vector_.capacity() == 0) return;
    std::for_each(vector_.begin(), vector_.end(),[](client_session::ptr &n){
        n->sock().cancel();
        n->sock().close();
    });
    vector_.clear();
}



} //client
} //toster
