#include "client_manager.h"
#include <algorithm>

namespace toster {
namespace client {

client_manager::client_manager(
        boost::asio::io_service &service):
        service_(service){
}

void client_manager::init(const toster::client_config &conf){
    ep_ = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(conf.address), int(conf.port));
    on_connected_sessions_ = 0;
    sessions_count = conf.sessions;
    data_size = conf.data_size;
    test_attempts = conf.test_attempts;
}

void client_manager::create_sessions(){
    for (int16_t i = 0; i < sessions_count; ++i){
        client_session * sess = new client_session(service_,data_size,test_attempts);
        client_session::ptr shared_sess(sess);
        sessions_.push_back(shared_sess);
        array_statistics.push_back(sess->statistic());
    }
}

void client_manager::connect_sessions() {
    for(auto a = sessions_.begin();a != sessions_.end();a++) {
        if (a == sessions_.begin())
                conn_start = boost::posix_time::microsec_clock::universal_time();
        (*a)->sock().async_connect(ep_,[&,a](const error_code& err){
            if(err){
                (*a)->set_conn_error();
                //std::cerr << "error:" << err.message() << '\n';
            }
            else{
                (*a)->set_conn_success();
                //std::cout << "succses conn: " << boost::this_thread::get_id() << '\n';
            }
            boost::recursive_mutex::scoped_lock lk(this->manager_mutex);
            if (++on_connected_sessions_ == sessions_count){
                conn_end = boost::posix_time::microsec_clock::universal_time();
                conn_duration = (conn_end - conn_start).total_milliseconds();
                std::cout << "duration: " << this->conn_duration << '\n';
                //run_test();
                work_ = new boost::asio::io_service::work(service_);
            }
        });
    }
}

void client_manager::run_test(){
    boost::recursive_mutex::scoped_lock lk(this->manager_mutex);
    if((on_connected_sessions_ != sessions_count) || sessions_.empty()){
        std::cerr << "test error: not created sessions" << '\n';
        return;
    }
    std::for_each(sessions_.begin(),sessions_.end(),[](client_session::ptr &a){
        a->run();
    });
    std::cerr << "testing " << sessions_count << " sessions, witn " << data_size
              << " bytes bunch of random data "<< test_attempts << " times" << '\n';
}

std::string client_manager::sessions_statistic(){
    std::stringstream stream_;
    stream_ << "session id;isconnected;attempts succes;"
        << "attempts fault;slowest attempt;fastest attempt;medium attempt" << '\n';
    std::for_each(sessions_.begin(), sessions_.end(),
    [&stream_,this](client_session::ptr &n){
        std::sort(n->statistic().echo_time.begin(),n->statistic().echo_time.end());
        stream_ << n->statistic().id << ";"
                << n->statistic().is_connected << ";"
                << n->statistic().attempts_succes << ";"
                << n->statistic().attempts_fault << ";"
                << n->statistic().echo_time.back() << ";"
                << n->statistic().echo_time[this->test_attempts / 2]
                << '\n';
    });
    std::string out = stream_.str();
    return out;
}



void client_manager::delete_sessions(){
    boost::recursive_mutex::scoped_lock lk(manager_mutex);
    if(sessions_.capacity() == 0) return;
    std::for_each(sessions_.begin(), sessions_.end(),[](client_session::ptr &n){
        n->close();
    });
    //array_statistics.erase(array_statistics.begin(),array_statistics.end());
    sessions_.erase(sessions_.begin(),sessions_.end());
    on_connected_sessions_ = 0;
}


} //client
} //toster
