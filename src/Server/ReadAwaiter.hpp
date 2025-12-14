#pragma once
#include <coroutine>
#include <vector>
#include <memory>

class Reactor;

class ReadAwaiter {
public:
    ReadAwaiter(int fd, std::shared_ptr<Reactor> reactor, std::vector<uint8_t>& buffer);

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h);
    ssize_t await_resume();

private:
    int m_fd;
    std::shared_ptr<Reactor> m_reactor;
    std::vector<uint8_t>& m_buffer;
};
