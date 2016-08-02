#include "common_tools.h"
#include <ctime>
#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>

namespace toster {

std::string get_date(void) {

    //std::time_t* result;
    std::time_t res;
    res = std::time(nullptr);
    std::tm* tm_result = std::localtime(&res);
    char buff[70];
    size_t size = std::strftime(buff, sizeof(buff), "%Y%m%d", tm_result);
    std::string date(buff, size);
    return date;
}

std::string get_time(void) {

    //std::time_t* result;
    std::time_t res;
    res = std::time(nullptr);
    std::tm* tm_result = std::localtime(&res);
    char buff[70];
    size_t size = std::strftime(buff, sizeof(buff), "%H%M%S", tm_result);
    std::string time(buff, size);
    return time;
}

server_config::server_config(const std::string &filename){
    try {
        boost::property_tree::ptree tree;
        boost::property_tree::read_xml(filename, tree);
        threads = tree.get("tcptoster_config.server.threads", 0);
        port = tree.get("tcptoster_config.server.port", 0);
    }
    catch (std::exception &e){
        std::cerr << "Error: " << e.what() << "\n";
    }
}

client_config::client_config(const std::string &filename){
    try {
        boost::property_tree::ptree tree;
        boost::property_tree::read_xml(filename, tree);
        address = tree.get<std::string>("tcptoster_config.client.address");
        threads = tree.get("tcptoster_config.client.threads", 0);
        port = tree.get("tcptoster_config.client.port", 0);
        sessions = tree.get("tcptoster_config.client.sessions", 0);
        data_size = tree.get("tcptoster_config.client.data_size", 0);
        data_queue = tree.get("tcptoster_config.client.data_queue", 0);
    }
    catch (std::exception &e){
        std::cerr << "Error: " << e.what() << "\n";
    }
}

} //toster
