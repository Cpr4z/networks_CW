#include <future>
#include <coroutine>

template<typename T = void>
class Task {
public:
    struct promise_type {
        T value_;
        std::exception_ptr eptr_;

        Task get_return_object() noexcept {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() noexcept { eptr_ = std::current_exception(); }

        template<class U>
        requires std::convertible_to<U, T>
        void return_value(U&& v) noexcept(std::is_nothrow_constructible_v<T,U&&>) {
            value_ = std::forward<U>(v);
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    Task() = default;
    explicit Task(handle_type h) : h_(h) {}
    Task(Task&& other) noexcept : h_(std::exchange(other.h_, {})) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) { if (h_) h_.destroy(); h_ = std::exchange(other.h_, {}); }
        return *this;
    }
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    ~Task() { if (h_) h_.destroy(); }

    T get() {
        if (!h_) throw std::runtime_error("empty Task");
        if (h_.promise().eptr_) std::rethrow_exception(h_.promise().eptr_);
        return std::move(h_.promise().value_);
    }

private:
    handle_type h_;
};

template<>
class Task<void> {
public:
    struct promise_type {
        std::exception_ptr eptr_;

        Task get_return_object() noexcept {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }

        void unhandled_exception() noexcept { eptr_ = std::current_exception(); }
        void return_void() noexcept {}
    };

    using handle_type = std::coroutine_handle<promise_type>;

    Task() = default;
    explicit Task(handle_type h) : h_(h) {}
    Task(Task&& other) noexcept : h_(std::exchange(other.h_, {})) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) { if (h_) h_.destroy(); h_ = std::exchange(other.h_, {}); }
        return *this;
    }
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    ~Task() { if (h_) h_.destroy(); }

    void get() {
        if (!h_) throw std::runtime_error("empty Task");
        if (h_.promise().eptr_) std::rethrow_exception(h_.promise().eptr_);
    }

private:
    handle_type h_;
};