#pragma once

#include <coroutine>

#include <BasicTypes.hpp>

class Reactor {
public:
    Reactor();
    ~Reactor();

    // Чтение
    void addReadEvent(int fd, std::coroutine_handle<> h);
    void removeReadEvent(int fd);

    // Запись
    void addWriteEvent(int fd, std::coroutine_handle<> h);
    void removeWriteEvent(int fd);

//    void addReadEvent(int fd, std::coroutine_handle<> h);
//    void removeEvent(int fd);
    void run();
    int getKqueue() const { return m_kq; }

private:
    int m_kq;
    UMap<int, std::coroutine_handle<>> m_read_handlers;
    UMap<int, std::coroutine_handle<>> m_write_handlers;
};