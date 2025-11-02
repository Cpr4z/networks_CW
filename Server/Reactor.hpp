#pragma once

#include <coroutine>

#include <BasicTypes.hpp>

class Reactor {
public:
    Reactor();
    ~Reactor();

    void addReadEvent(int fd, std::coroutine_handle<> h);
    void removeEvent(int fd);
    void run();

private:
    int m_kq;
    UMap<int, std::coroutine_handle<>> m_handlers;
};