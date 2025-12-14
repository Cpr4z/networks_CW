#pragma once
#include <coroutine>
#include <vector>
#include <memory>

class Reactor;

class WriteAwaiter {
public:
    WriteAwaiter(int fd, std::shared_ptr<Reactor> reactor,
                 const std::vector<uint8_t>& data);

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h);
    ssize_t await_resume();

private:
    int m_fd;
    std::shared_ptr<Reactor> m_reactor;
    const std::vector<uint8_t>& m_data;
    size_t m_sent = 0;
};
