#pragma once

#include <arpa/inet.h>

#include <coroutine>
#include <memory>

class Reactor;

class AcceptAwaiter {
public:
    explicit AcceptAwaiter(int fd, const std::shared_ptr<Reactor>& reactor) noexcept;

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h);
    int await_resume() const noexcept;

private:
    int m_server_fd = 0;
    std::shared_ptr<Reactor> m_reactor;
};
