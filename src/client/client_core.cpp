#include "client_core.h"
#include <boost/bind.hpp>
#include <fstream>
#include <algorithm>
#include "client_manager.h"

namespace toster {
namespace client {

client_core::client_core(const client_config &conf):
    service(),
    signals_(service),
    manager_(service){
    init(conf);
}

client_core::client_core():
    service(),
    signals_(service),
    manager_(service),
    is_init(false){}

void client_core::init(const client_config &conf) {
    threads_count = conf.threads;
    manager_.init(conf);
    is_init = true;
    std::cout << "clients initialized" << '\n';
}

void client_core::start_client(){
    //assign signal
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    #if defined(SIGQUIT)
        signals_.add(SIGQUIT); // defined(SIGQUIT)
    #endif
    signals_.async_wait([this](const error_code& err, int num){
        this->close();
    });
    connect();
    //creation threads
    boost::thread thread_pool[threads_count];
    for (int16_t i = 0; i < threads_count; i++)
        thread_pool[i] = boost::thread([this](){
            this->thread_listen();
        });
    for (int16_t i = 0; i < threads_count; i++){
        //thread_pool[i].join();
        thread_pool[i].detach();
    }
    std::cout << "client started" << '\n';
}

void client_core::connect() {
    manager_.create_sessions();
    manager_.connect_sessions();
}

void client_core::disconnect(){
    manager_.delete_sessions();
}

void client_core::close(){
    disconnect();
    service.stop();
}

void client_core::run_test() {
    if (!is_init){
        std::cout << "client not initialized" << '\n';
        return;
    }
    manager_.run_test();
}

void client_core::get_log(){
    std::string log_name = "log_clinet_";
    log_name.append(get_date());
    log_name.append(1,'_');
    log_name.append(get_time());
    log_name.append(std::string(".txt"));
    std::fstream report(log_name, std::ios::out);
    report << "client statistic " << get_date() << " - " << get_time() << '\n'
        << "threads: " << threads_count  << '\n'
        << "sessions: " << manager_.get_sessions()  << '\n'
        << "test data size: " << manager_.get_data_size()  << " bytes" << '\n'
        << "test attemps pro session: " << manager_.get_test_attempts() << '\n'
        << "all sessions coonnnection time: "
        << manager_.get_conn_duration() << " msec" << '\n';
    report << manager_.sessions_statistic();
    report.close();
    std::cout << "logged in " << log_name << '\n';
}

void client_core::thread_listen() {
        std::cout
        << "thread run: "
        << boost::this_thread::get_id() << '\n';
    service.run();
}

} //client
} //toster
