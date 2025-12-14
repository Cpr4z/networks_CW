//#include "Server.hpp"
#include <string>
#include <csignal>

#include <iostream>
#include <csignal>

#include "SyncServer.hpp"

//namespace {
//    using namespace std::string_view_literals;
//    constexpr std::string_view CONFIG_PATH = "../config/config.yaml"sv;
//}
//
//std::unique_ptr<NoteServer> g_server;
//
//void handleSignal(int) {
//    if (g_server) {
//        g_server->stop();
//    }
//}
//
//
//int main(int argc, char** argv) {
//    int port = 8080;
//    std::string host = "127.0.0.1";
//
//    if (argc >= 2) port = std::stoi(argv[1]);
//    if (argc >= 3) host = argv[2];
//
//    std::cout<< "Starting service with params " << host << ":" << port << '\n';
//
//    std::signal(SIGINT, handleSignal);
//    std::signal(SIGTERM, handleSignal);
//
//    g_server = std::make_unique<NoteServer>(port, host);
//    auto task = g_server->runAsync();
//    g_server->getReactor()->run();
//
//    std::cout << "Server is running\n";
//
//    return 0;
//}
//

std::atomic<bool> running{true};

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    running = false;
}

int main() {
    // Установка обработчика сигналов
    signal(SIGINT, signal_handler);

    SyncServer server(8080, "127.0.0.1");

    // Запуск сервера в отдельном потоке
    std::thread server_thread([&server]() {
        server.run();
    });

    // Ожидание завершения
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Остановка сервера
    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }

    std::cout << "Server shutdown complete" << std::endl;
    return 0;
}
