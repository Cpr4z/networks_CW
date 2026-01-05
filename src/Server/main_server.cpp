#include <csignal>
#include <iostream>

#include "Server.hpp"

std::atomic<bool> running{true};

void signal_handler(int signal) {
    running = false;
}

int main() {
    signal(SIGINT, signal_handler);

#ifdef __APPLE__
    Server server(8080, "127.0.0.1");
#elif __linux__
    Server server(8080, "0.0.0.0");
#endif

    server.setEncryptionKey("shared-secret-key-123");

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

    return 0;
}
