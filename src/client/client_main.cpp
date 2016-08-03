#include "client_core.h"

int main(int argc, char* argv[]) {
    try {
        toster::client_config conf("tcptoster_config.xml");
        toster::client::client_core client(conf);
        client.start_client();
        std::cout << "enter cmd:" << std::endl;
        while (true) {
            std::string msg;
            std::getline(std::cin, msg);
            if (msg == "conn")
                client.connect();
            else if (msg == "disconn")
                client.disconnect();
            else if (msg == "test")
                client.run_test();
            else if (msg == "logfile")
                client.log_file();
            else if (msg == "logconsole")
                client.log_console();
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
