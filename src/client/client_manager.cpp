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
    test_attempts = conf.test_attempts;
}

void client_manager::create_sessions(){
    for (int16_t i = 0; i < sessions_count; ++i){
        client_session * sess = new client_session(service_,data_size,test_attempts,ep_);
        client_session::ptr shared_sess(sess);
        sessions_.push_back(shared_sess);
        array_statistics.push_back(sess->statistic());
    }
}

void client_manager::connect_sessions() {
    this->conn_start = boost::posix_time::microsec_clock::universal_time();
    std::for_each(sessions_.begin(),sessions_.end(),
        [](client_session::ptr &a){
        a->connect();
    });
    work_ = new boost::asio::io_service::work(service_);
}

void client_manager::run_test(){
    boost::recursive_mutex::scoped_lock lk(this->manager_mutex);
    if(sessions_.empty()){
        std::cerr << "test error: not created sessions" << '\n';
        return;
    }
    this->test_start = boost::posix_time::microsec_clock::universal_time();
    std::for_each(sessions_.begin(),sessions_.end(),[](client_session::ptr &a){
        a->run();
    });
    std::cerr << "testing " << sessions_count << " sessions, witn " << data_size
              << " bytes bunch of random data "<< test_attempts << " times" << '\n';
}

long client_manager::get_conn_duration(){
    boost::posix_time::ptime conn_end = this->conn_start;
    std::for_each(sessions_.begin(),sessions_.end(),
        [&conn_end](client_session::ptr &a){
        if (a->statistic().conn_time > conn_end)
            conn_end = a->statistic().conn_time;
    });
    return (conn_end - this->conn_start).total_milliseconds();
}

long client_manager::get_test_duration(){
    boost::posix_time::ptime test_end = this->test_start;
    std::for_each(sessions_.begin(),sessions_.end(),
        [&test_end](client_session::ptr &a){
        if (a->statistic().test_time > test_end)
            test_end = a->statistic().test_time;
    });
    return (test_end - this->test_start).total_milliseconds();
}

long client_manager::get_slowest_attempt(){
    long slowest = sessions_.front()->statistic().echo_time.back();
    std::for_each(sessions_.begin(),sessions_.end(),
        [&slowest,this](client_session::ptr &a){
        std::sort(a->statistic().echo_time.begin(),a->statistic().echo_time.end());
        if (a->statistic().echo_time.back() > slowest)
            slowest = a->statistic().echo_time.back();
    });
    return slowest;
}

long client_manager::get_fastest_attempt(){
    long fastest = sessions_.front()->statistic().echo_time.front();
    std::for_each(sessions_.begin(),sessions_.end(),
        [&fastest,this](client_session::ptr &a){
        std::sort(a->statistic().echo_time.begin(),a->statistic().echo_time.end());
        if (a->statistic().echo_time.front() < fastest)
            fastest = a->statistic().echo_time.front();
    });
    return fastest;
}

int16_t client_manager::get_success_sessions(){
    int16_t count;
    std::for_each(sessions_.begin(),sessions_.end(),
        [&count](client_session::ptr &a){
         if (a->statistic().attempts_fault == 0)
             count++;
    });
    return count;
}

std::string client_manager::sessions_statistic(){
    std::stringstream stream_;
    stream_ << "session id; isconnected; attempts succes; "
        << "attempts fault; slowest attempt(msec); medium attempt(msec); fastest attempt(msec)" << '\n';
    std::for_each(sessions_.begin(), sessions_.end(),
    [&stream_,this](client_session::ptr &n){
        std::sort(n->statistic().echo_time.begin(),n->statistic().echo_time.end());
        stream_ << n->statistic().id << "; "
                << n->statistic().is_connected << "; "
                << n->statistic().attempts_succes << "; "
                << n->statistic().attempts_fault << "; "
                << n->statistic().echo_time.back() << "; "
                << n->statistic().echo_time[this->test_attempts / 2] << "; "
                << n->statistic().echo_time.front()
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
    sessions_.erase(sessions_.begin(),sessions_.end());
}

} //client
} //toster
