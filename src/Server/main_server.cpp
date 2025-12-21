#include <csignal>
#include <iostream>

#include "Server.hpp"

std::atomic<bool> running{true};

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    running = false;
}

int main() {
    signal(SIGINT, signal_handler);

    Server server(8080, "127.0.0.1");
//    SyncServer server(8080, "0.0.0.0");

    std::thread server_thread([&server]() {
        server.run();
    });

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }

    std::cout << "Server shutdown complete" << std::endl;
    return 0;
}
