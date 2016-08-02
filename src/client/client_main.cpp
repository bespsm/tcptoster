#include "client_core.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        toster::client_config conf("tcptoster_config.xml");
        toster::client::client_core client(conf);
        client.connect();
        std::cout << "enter cmd:" << std::endl;
        while (true) {
            std::string msg;
            std::getline(std::cin, msg);
            if (msg == "connect")
                client.connect();
            else if (msg == "disconnect")
                client.disconnect();
            else if (msg == "test")
                client.run_test();
            else if (msg == "isrun")
                std::cout << client.is_run() << std::endl;
            else if (msg == "init"){
                toster::client_config newconf("tcptoster_config.xml");
                client.init(newconf);
            }
            else if (msg == "close" || msg == "exit"){
                client.close();
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
