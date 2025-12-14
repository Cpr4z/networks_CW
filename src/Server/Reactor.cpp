#include "Reactor.hpp"
#include <iostream>
#include <unistd.h>
#include <cerrno>

#include <sys/event.h>

Reactor::Reactor() {
    m_kq = kqueue();
    if (m_kq < 0) {
        perror("kqueue");
        std::exit(1);
    }
}

Reactor::~Reactor() {
    close(m_kq);
}

void Reactor::addReadEvent(int fd, std::coroutine_handle<> h) {
    std::cout << "Reactor::addReadEvent" << std::endl;
    // Сохраняем хэндл корутины
    m_read_handlers[fd] = h;

    struct kevent ev;
    // EV_ONESHOT автоматически удалит событие после срабатывания
    EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, (void*)h.address());

    if (kevent(m_kq, &ev, 1, nullptr, 0, nullptr) == -1) {
        perror("kevent add read");
        m_read_handlers.erase(fd);
    }
}

void Reactor::addWriteEvent(int fd, std::coroutine_handle<> h) {
    // Сохраняем хэндл корутины
    m_write_handlers[fd] = h;

    struct kevent ev;
    // EV_ONESHOT автоматически удалит событие после срабатывания
    EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, (void*)h.address());

    if (kevent(m_kq, &ev, 1, nullptr, 0, nullptr) == -1) {
        perror("kevent add write");
        m_write_handlers.erase(fd);
    }
}

void Reactor::removeReadEvent(int fd) {
    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    kevent(m_kq, &ev, 1, nullptr, 0, nullptr);

    m_read_handlers.erase(fd);
}

void Reactor::removeWriteEvent(int fd) {
    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
    kevent(m_kq, &ev, 1, nullptr, 0, nullptr);

    m_write_handlers.erase(fd);
}

// Старая функция для обратной совместимости
//void Reactor::removeEvent(int fd) {
//    removeReadEvent(fd);
//}

void Reactor::run() {
    std::vector<struct kevent> events(64);

    while (true) {
        int n = kevent(m_kq, nullptr, 0, events.data(), (int)events.size(), nullptr);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("kevent wait");
            break;
        }

        for (int i = 0; i < n; ++i) {
            auto* handle_ptr = static_cast<std::coroutine_handle<>*>(events[i].udata);
            if (handle_ptr && *handle_ptr) {
                std::coroutine_handle<> h = *handle_ptr;

                // Удаляем событие из соответствующих мап
                int fd = static_cast<int>(events[i].ident);

                if (events[i].filter == EVFILT_READ) {
                    auto it = m_read_handlers.find(fd);
                    if (it != m_read_handlers.end() && it->second.address() == h.address()) {
                        m_read_handlers.erase(it);
                    }
                } else if (events[i].filter == EVFILT_WRITE) {
                    auto it = m_write_handlers.find(fd);
                    if (it != m_write_handlers.end() && it->second.address() == h.address()) {
                        m_write_handlers.erase(it);
                    }
                }

                // Возобновляем корутину
                h.resume();
            }
        }
    }
}