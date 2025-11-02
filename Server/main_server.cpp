//#include <string_view>
#include "Server.hpp"
#include <string>
#include <csignal>

#include <iostream>

namespace {
    using namespace std::string_view_literals;
    constexpr std::string_view CONFIG_PATH = "../config/config.yaml"sv;
}

std::unique_ptr<NoteServer> g_server;

void handleSignal(int) {
    if (g_server) {
        g_server->stop();
    }
}


int main(int argc, char** argv) {
    int port = 8080;
    std::string host = "127.0.0.1";

    if (argc >= 2) port = std::stoi(argv[1]);
    if (argc >= 3) host = argv[2];

    std::cout<< "Starting service with params " << host << ":" << port << '\n';

    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    g_server = std::make_unique<NoteServer>(port, host);
    auto task = g_server->runAsync();
    g_server->getReactor()->run();

    std::cout << "Server is running\n";

    return 0;
}
