#include "server_core.h"


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
            else if (msg == "logfile")
                serv.log_file();
            else if (msg == "logconsole")
                serv.log_console();
            else if (msg == "init"){
                toster::server_config newconf("tcptoster_config.xml");
                serv.init(newconf);
            }
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
