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

void client_core::connect() {
    manager_.create_sessions();
    manager_.connect_sessions();

    //creation threads
    boost::thread thread_pool[threads_count];
    for (int16_t i = 0; i < threads_count; i++)
        thread_pool[i] = boost::thread([this](){
            this->thread_listen();
        });
    for (int16_t i = 0; i < threads_count; i++){
        //threads[i]->join();
        thread_pool[i].detach();
    }
    std::cout << "client started" << '\n';
}

void client_core::run_test() {
    if (!is_init){
        std::cout << "client not initialized" << '\n';
        return;
    }
    std::cout << "main thread: "
        << boost::this_thread::get_id() << '\n';
    // creating sessions
}

void client_core::disconnect() {
    manager_.disconnect_sessions();
    std::cout << "all clients disconnected" << std::endl;
}


void client_core::close(){
    disconnect();
    service.stop();
}

/*
void client_core::get_log(){
    boost::recursive_mutex::scoped_lock lk(core_mutex);
    if(sessions.capacity() == 0) return;
    std::string log_name = "logserv";
    log_name.append(get_date());
    log_name.append(1,'_');
    log_name.append(get_time());
    log_name.append(std::string(".txt"));
    std::fstream report(log_name, std::ios::out);
    std::for_each(sessions.begin(), sessions.end(),[&report](client_session::ptr &n){
        auto stat = n->statistic();
        report
            << "state(0-run,1-stp,2-flt): " << stat.state
            << "; id: " << stat.id
            << "; bytes read: " << stat.bytes_r
            << "; bytes write: " << stat.bytes_w
            << "; last error: " << stat.last_error.message()
            << "; error quantity: " << stat.error_quantity
            << '\n';
    });
    report.close();
    std::cout << "logged in " << log_name << '\n';
}*/

void client_core::thread_listen() {
        std::cout
        << "thread run: "
        << boost::this_thread::get_id() << '\n';
    service.run();
}

} //client
} //toster
