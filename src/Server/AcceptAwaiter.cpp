#include "AcceptAwaiter.hpp"

#include <iostream>

#include <fcntl.h>

#include "Reactor.hpp"

AcceptAwaiter::AcceptAwaiter(int fd, const std::shared_ptr<Reactor>& reactor) noexcept
    : m_server_fd(fd), m_reactor(reactor) {

}

void AcceptAwaiter::await_suspend(std::coroutine_handle<> h) {
    std::cout << "AcceptAwaiter::await_suspend" << std::endl;
    if (!m_reactor) {
        std::cerr << "Reactor not set for AcceptAwaiter\n";
        return;
    }
    m_reactor->addReadEvent(m_server_fd, h);
}

int AcceptAwaiter::await_resume() const noexcept {
    std::cout << "Entered await_resume in AcceptAwaiter::await_resume" << std::endl;
    sockaddr_in client_addr{};
    socklen_t len = sizeof(client_addr);

    int client_fd = accept(m_server_fd, (sockaddr*)&client_addr, &len);
    std::cout << client_fd << std::endl;
    if (client_fd < 0) {
        perror("accept");
        return -1;
    }

    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip, INET_ADDRSTRLEN);
    std::cout << "Новое соединение от " << ip
              << ":" << ntohs(client_addr.sin_port)
              << " (fd=" << client_fd << ")" << std::endl;

    return client_fd;
}