#pragma once

#include <chrono>
#include <coroutine>
#include <exception>
#include <string>
#include <type_traits>
#include <vector>

struct Promise;

struct CoroTask {
    using promise_type = Promise;

    bool await_ready();
    void await_suspend(std::coroutine_handle<Promise> suspended) const;
    void await_resume() {}

    void update(float delta);

    std::coroutine_handle<Promise> handle;
};

struct Promise {
    template <class... Args>
    explicit Promise(std::string functionName, Args&&...)
        : functionName(std::move(functionName))
    { }

    CoroTask get_return_object();
    std::suspend_never initial_suspend() noexcept { return {}; };
    std::suspend_always final_suspend() noexcept { return {}; };
    void unhandled_exception();
    void return_void() const;

    std::exception_ptr exception;

    std::coroutine_handle<Promise> root;
    std::coroutine_handle<Promise> leaf;
    std::coroutine_handle<Promise> parent;
    float time = 0.f;
    std::string functionName;
};

struct WaitFor {
};