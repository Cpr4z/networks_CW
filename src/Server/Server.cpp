#include "Server.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "AcceptAwaiter.hpp"
#include "ReadAwaiter.hpp"

#include <iostream>

#include <Protocol.hpp>

NoteServer::NoteServer(int port, const std::string& host)
    : m_port(port), m_host(host) {
    m_reactor = std::make_shared<Reactor>();
    m_reactor_thread = std::thread(&NoteServer::startReactor, this);
    m_repository = Repository::instance();
}

void NoteServer::handleAuthRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    // Получаем данные пользователя, проверяем по айдишнику есть ли такой пользователь, если нет, то возвращаем соответствующий результат
    const auto& request = Protocol::decodeAuthRequest(buffer);
//    else {
//        m_repository->addUser(client_fd, request->login, request->password);
//    }
}

void NoteServer::handleRegistrationRequest(int client_fd, const std::vector<uint8_t>& buffer) {
    const auto& request = Protocol::decodeRegistrationRequest(buffer);
}

size_t NoteServer::getMessageLength(Protocol::Operation op,
                                    const std::vector<uint8_t>& buffer,
                                    size_t offset) {
    switch (op) {
        case Protocol::Operation::AUTH: {
            if (buffer.size() - offset < 6) return 0;  // Минимум 6 байт

            uint16_t loginLen, passwordLen;
            std::memcpy(&loginLen, buffer.data() + offset + 2, sizeof(uint16_t));
            std::memcpy(&passwordLen, buffer.data() + offset + 4, sizeof(uint16_t));

            return 6 + loginLen + passwordLen;  // 2(op) + 2(loginLen) + 2(passwordLen) + login + password
        }
        case Protocol::Operation::REGISTRATION: {
            // Аналогично AUTH
            if (buffer.size() - offset < 6) return 0;

            uint16_t loginLen, passwordLen;
            std::memcpy(&loginLen, buffer.data() + offset + 2, sizeof(uint16_t));
            std::memcpy(&passwordLen, buffer.data() + offset + 4, sizeof(uint16_t));

            return 6 + loginLen + passwordLen;
        }
            // Добавьте другие операции
        default:
            return 2;  // Минимум код операции
    }
}

void NoteServer::onClientDisconnected(int client_fd) {
}

void NoteServer::processClientMessage(int client_fd,
                                      const std::vector<uint8_t>& message) {
    const Protocol::Operation op = Protocol::decodeOperation(message);
    try {
        switch (op) {
            case Protocol::Operation::AUTH: {
                handleAuthRequest(client_fd, message);
                break;
            }
            case Protocol::Operation::REGISTRATION: {
                handleRegistrationRequest(client_fd, message);
                break;
            }
            default:
                std::cerr << "Unknown operation from client " << client_fd
                          << ": " << static_cast<uint16_t>(op) << std::endl;
                // Можно отправить NACK
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing message from client " << client_fd
                  << ": " << e.what() << std::endl;
    }
}

Task<void> NoteServer::processClientMessages(int client_fd) {
    std::vector<uint8_t> buffer;
    while (true) {
        // Асинхронно ждем данные от клиента
        ssize_t received = co_await ReadAwaiter(client_fd, m_reactor, buffer);

        if (received <= 0) {
            // Клиент отключился
            std::cout << "Client " << client_fd << " disconnected" << std::endl;
            co_return;
        }

        // Обрабатываем все полные сообщения в буфере
        size_t processed = 0;
        while (buffer.size() - processed >= 2) {  // Минимум 2 байта для кода операции
            // Декодируем код операции
            uint16_t opCode;
            std::memcpy(&opCode, buffer.data() + processed, sizeof(uint16_t));
            auto operation = static_cast<Protocol::Operation>(opCode);

            // Определяем длину сообщения (зависит от типа операции)
            size_t messageLength = getMessageLength(operation, buffer, processed);

            if (messageLength == 0 || buffer.size() - processed < messageLength) {
                // Неполное сообщение, ждем еще данных
                break;
            }

            // Извлекаем полное сообщение
            std::vector<uint8_t> message(
                    buffer.begin() + processed,
                    buffer.begin() + processed + messageLength
            );

            // Обрабатываем сообщение
            processClientMessage(client_fd, buffer);

            processed += messageLength;
        }

        // Удаляем обработанные данные из буфера
        if (processed > 0) {
            buffer.erase(buffer.begin(), buffer.begin() + processed);
        }
    }
}


Task<void> NoteServer::handleClient(int client_fd) {
    std::cout << "Starting client handler for fd: " << client_fd << std::endl;

    try {
        co_await processClientMessages(client_fd);
    } catch (const std::exception& e) {
        std::cerr << "Error handling client " << client_fd << ": " << e.what() << std::endl;
    }

    onClientDisconnected(client_fd);
    close(client_fd);
    co_return;
}

Task<void> NoteServer::runAsync() {
    m_running = true;
    m_server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd == -1) {
        perror("socket");
        co_return;
    }

    int opt = 1;
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    inet_pton(AF_INET, m_host.c_str(), &addr.sin_addr);

    if (bind(m_server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(m_server_fd);
        co_return;
    }

    if (listen(m_server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(m_server_fd);
        co_return;
    }

    int flags = fcntl(m_server_fd, F_GETFL, 0);
    fcntl(m_server_fd, F_SETFL, flags | O_NONBLOCK);

    std::cout << "Server is running\n";
    std::vector<Task<void>> client_tasks;
    while (m_running) {
        int client_fd = co_await AcceptAwaiter(m_server_fd, m_reactor);
        if (client_fd >= 0) {
            client_tasks.push_back(handleClient(client_fd));
            // Можно запустить отдельную корутину для клиента:
//             co_spawn(handleClient(client_fd));
            std::cout << "Before hanling client" << std::endl;
//            handleClient(client_fd);
        }
    }
}

Id NoteServer::getNextUserId() {
    return m_users.size() + 1;
}

Id NoteServer::getNextDocId() {
    return m_users.size() + 1;
}

void NoteServer::stop() {
    m_running = false;
    if (m_server_fd >= 0) {
        close(m_server_fd);
        m_server_fd = -1;
    }
}

void NoteServer::createDocument() {
    Id new_id = getNextDocId();
    std::lock_guard<std::mutex> lock(m_mutex);
//    m_documents.insert({new_id, std::make_shared<Note>(new_id)});
}

//void NoteServer::createUser(const std::string& name) {
//    Id new_id = getNextUserId();
//    std::lock_guard<std::mutex> lock(m_mutex);
//    m_users.insert({new_id, std::make_shared<User>(name, new_id)});
//}

void NoteServer::closeDocument(Id id) {
    std::lock_guard<std::mutex> lock(m_mutex);
//    m_documents.erase(id);
}

void NoteServer::deleteUser(Id id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_users.erase(id);
}