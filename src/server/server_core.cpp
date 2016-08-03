#include "server_core.h"
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <algorithm>
#include <common_tools.h>

namespace toster {
namespace server {

server_core::server_core(const server_config &conf):
    service(),
    signals_(service),
    acceptor(service),
    is_init(true){
    init(conf);
}

server_core::server_core():
    port(8001),
    thread_count(1),
    service(),
    signals_(service),
    acceptor(service),
    is_init(false){}

void server_core::init(const server_config &conf) {
    thread_count = conf.threads;
    port = conf.port;
    is_init = true;
    std::cout << "server initialized" << '\n';
}

void server_core::start() {
    if (!is_init){
        std::cout << "server not initialized" << '\n';
        return;
    }
    std::cout
        << "main thread: "
        << boost::this_thread::get_id() << '\n';

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
    acceptor.open(endpoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen();

    boost::thread thread_pool[thread_count];
    for (size_t i = 0; i < thread_count; i++)
        thread_pool[i] = boost::thread(boost::bind(&server_core::thread_listen,this));
    // Wait for all threads in the pool to exit.
    for (size_t i = 0; i < thread_count; i++){
        //threads[i]->join();
        thread_pool[i].detach();
    }
    std::cout << "server started" << '\n';
}


void server_core::stop() {
    kill_sessions();
    boost::recursive_mutex::scoped_lock lk(core_mutex);
    service.stop();
    acceptor.close();
    std::cout << "server stopped" << std::endl;
}

void server_core::status(){
    boost::recursive_mutex::scoped_lock lk(core_mutex);
    if(sessions.capacity() == 0) return;
    std::for_each(sessions.begin(), sessions.end(),[](tcp_session::ptr &n){
        auto stat = n->statistic();
        std::cout
            << "state(0-run,1-stp,2-flt): " << stat.state
            << "; id: " << stat.id
            << "; bytes read: " << stat.bytes_r
            << "; bytes write: " << stat.bytes_w
            << "; last error: " << stat.last_error.message()
            << "; error quantity: " << stat.error_quantity
            << '\n'; });
}


void server_core::get_log(){
    boost::recursive_mutex::scoped_lock lk(core_mutex);
    if(sessions.capacity() == 0) return;
    std::string log_name = "log_serv_";
    log_name.append(get_date());
    log_name.append(1,'_');
    log_name.append(get_time());
    log_name.append(std::string(".txt"));
    std::fstream report(log_name, std::ios::out);
    report
        << "state(0-run,1-stp,2-flt)"
        << ";id"
        << ";bytes read"
        << ";bytes write"
        << ";last error"
        << ";error quantity"
        << '\n';
    std::for_each(sessions.begin(), sessions.end(),[&report](tcp_session::ptr &n){
        auto stat = n->statistic();
        report
            << stat.state << ";"
            << stat.id << ";"
            << stat.bytes_r << ";"
            << stat.bytes_w << ";"
            << stat.last_error.message() << ";"
            << stat.error_quantity
            << '\n';
    });
    report.close();
    std::cout << "logged in " << log_name << '\n';
}


void server_core::kill_sessions() {
    boost::recursive_mutex::scoped_lock lk(core_mutex);
    if(sessions.capacity() == 0) return;
    std::for_each(sessions.begin(), sessions.end(),[](tcp_session::ptr &n){
        n->stop();
    });
    sessions.clear();
}

void server_core::handle_accept(tcp_session* client,const error_code &err) {
    //std::cout << "handle_accept " << client.use_count() << '\n';
    if(!err) {
        tcp_session::ptr shared_sess(client);
        boost::recursive_mutex::scoped_lock lk(core_mutex);
        sessions.push_back(shared_sess);
        shared_sess->start();
        std::cout << boost::this_thread::get_id() << ": new session" << std::endl;
    }
    else {
        delete client;
        std::cerr << err.message() << '\n';
    }

    tcp_session * sess_ = new tcp_session(service,sessions,core_mutex);
    acceptor.async_accept(sess_->sock(), boost::bind(&server_core::handle_accept,this,sess_,_1));
}

void server_core::thread_listen() {
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    #if defined(SIGQUIT)
        signals_.add(SIGQUIT);
    #endif // defined(SIGQUIT)
    signals_.async_wait(boost::bind(&server_core::stop, this));

    std::cout
        << "started thread: "
        << boost::this_thread::get_id()
        << " port: " << port << '\n';
    tcp_session * sess_ = new tcp_session(service,sessions,core_mutex);
    acceptor.async_accept(sess_->sock(), boost::bind(&server_core::handle_accept,this,sess_,_1));
    service.run();
}

} //server
} //toster
