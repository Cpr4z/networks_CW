#include "WriteAwaiter.hpp"
#include "Reactor.hpp"

#include <sys/socket.h>
#include <sys/event.h>
#include <sys/socket.h>   // Для send()
#include <unistd.h>       // Для close() если нужно
#include <fcntl.h>        // Для fcntl()
#include <unistd.h>
#include <iostream>

WriteAwaiter::WriteAwaiter(int fd, std::shared_ptr<Reactor> reactor,
                           const std::vector<uint8_t>& data)
        : m_fd(fd), m_reactor(reactor), m_data(data) {}

void WriteAwaiter::await_suspend(std::coroutine_handle<> h) {
    if (m_reactor) {
        // Регистрируем сокет на запись (EVFILT_WRITE)
        struct kevent ev;
        EV_SET(&ev, m_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, (void*)h.address());

        if (kevent(m_reactor->getKqueue(), &ev, 1, nullptr, 0, nullptr) == -1) {
            perror("kevent add for write");
            // Если не удалось зарегистрировать, сразу возобновляем корутину
            h.resume();
        }
    } else {
        // Если нет реактора, сразу возобновляем
        h.resume();
    }
}

ssize_t WriteAwaiter::await_resume() {
    // Пытаемся отправить данные
    ssize_t sent = send(m_fd,
                        m_data.data() + m_sent,
                        m_data.size() - m_sent,
                        0);

    if (sent > 0) {
        m_sent += sent;

        // Если отправили не все данные, нужно снова зарегистрироваться на запись
        if (m_sent < m_data.size()) {
            // Установим флаг, что нужно продолжить отправку
            // В реальности нужно будет снова вызвать co_await
            // или вернуть специальное значение
            return sent; // Положительное значение, но не все отправлено
        }
    }

    // Обработка ошибок
    if (sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Сокет не готов, нужно ждать
            return 0;
        }
        perror("send");
    }

    return sent; // <0 - ошибка, 0 - EAGAIN, >0 - отправлено байт
}
