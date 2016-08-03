#include "server_core.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        toster::server_config serv_conf("tcptoster_config.xml");
        toster::server::server_core serv(serv_conf);
        serv.start();
        std::cout << "enter cmd:" << std::endl;
        while (true) {
            std::string msg;
            std::getline(std::cin, msg);
            if (msg == "start")
                serv.start();
            else if (msg == "stop")
                serv.stop();
            else if (msg == "log")
                serv.get_log();
            else if (msg == "stat")
                serv.status();
            else if (msg == "init")
                serv.init(serv_conf);
            else if (msg == "close" || msg == "exit"){
                serv.stop();
                return 0;}
            else
                std::cout << "error cmd" << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
