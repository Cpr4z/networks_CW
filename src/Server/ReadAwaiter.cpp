#include "ReadAwaiter.hpp"
#include "Reactor.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

ReadAwaiter::ReadAwaiter(int fd, std::shared_ptr<Reactor> reactor, std::vector<uint8_t>& buffer)
        : m_fd(fd), m_reactor(reactor), m_buffer(buffer) {}

void ReadAwaiter::await_suspend(std::coroutine_handle<> h) {
    std::cout << "ReadAwaiter::await_suspend" << std::endl;
    if (m_reactor) {
        m_reactor->addReadEvent(m_fd, h);
    }
}

ssize_t ReadAwaiter::await_resume() {
    char temp_buffer[4096];
    ssize_t n = recv(m_fd, temp_buffer, sizeof(temp_buffer), 0);

    if (n > 0) {
        m_buffer.insert(m_buffer.end(), temp_buffer, temp_buffer + n);
    }

    return n;
}
