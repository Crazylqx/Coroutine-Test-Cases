#include <coroutine>
#include <iostream>
#include <x86intrin.h>
#include <cstdint>
#include <cassert>

struct empty {};

template<typename T>
struct promise;

template<typename T=empty>
struct coroutine : std::coroutine_handle<promise<T>>
{
    using promise_type = ::promise<T>;
    operator T() { return this->promise().value_; }
};

template<typename T=empty>
struct promise
{
    T value_;
    coroutine<T> get_return_object() { return {coroutine<T>::from_promise(*this)}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    std::suspend_always yield_value(T value) { value_ = value; return {}; }
    void unhandled_exception() {}

    void* operator new(std::size_t n) noexcept
    {
        std::cout << "allocate size: " << n << std::endl;
        return ::operator new(n);
    }
};

coroutine<size_t> f(size_t n) {
    for (size_t i = 0; i < n; i++) {
        co_yield i;
    }
    co_return;
}

uint64_t get_cycles() {
    unsigned int _;
    return __rdtscp(&_);
}

void test(size_t n, size_t data[]) {
    uint64_t cycles_start = get_cycles();
    for (auto x = f(1000 * 1000); !x.done(); x.resume()) {
        assert(x == data[x]);
    };
    uint64_t cycles_end = get_cycles();
    std::cout << "cycles: " << cycles_end - cycles_start << std::endl;
}

void init(size_t n, size_t data[]) {
    for (auto x = f(1000 * 1000); !x.done(); x.resume()) {
        constexpr size_t size_x = sizeof(x);
        data[x] = x;
    };
}

void test_a(size_t n, size_t data[]) {
    uint64_t cycles_start = get_cycles();
    for (auto x = 0; x < n; x++) {
        assert(x == data[x]);
    };
    uint64_t cycles_end = get_cycles();
    std::cout << "cycles: " << cycles_end - cycles_start << std::endl;
}

