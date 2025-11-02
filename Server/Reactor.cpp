#include "Reactor.hpp"


#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <unistd.h>

Reactor::Reactor() {
    m_kq = kqueue();
    if (m_kq < 0) {
        perror("kqueue");
        std::exit(1);
    }
}

Reactor::~Reactor()
{
    close(m_kq);
}

void Reactor::addReadEvent(int fd, std::coroutine_handle<> h) {
    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void*)h.address());
    if (kevent(m_kq, &ev, 1, nullptr, 0, nullptr) == -1) {
        perror("kevent add");
    }
}

void Reactor::removeEvent(int fd) {
    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    kevent(m_kq, &ev, 1, nullptr, 0, nullptr);
}

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
                h.resume();
            }
        }
    }
}