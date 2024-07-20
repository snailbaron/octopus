#include "task.hpp"

#include <iostream>

bool CoroTask::await_ready()
{
    if (handle.done()) {
        //std::cerr << "destroying coroutine before await: " << handle.address() << "\n";
        handle.destroy();
        return true;
    }
    return false;
}

void CoroTask::await_suspend(std::coroutine_handle<Promise> suspended) const
{
    std::cerr << std::format(
        "{} awaits {}\n",
        suspended.promise().functionName, handle.promise().functionName);

    std::cerr << std::format(
        "setting {}'s leaf to {}\n",
        suspended.promise().root.promise().functionName,
        handle.promise().leaf.promise().functionName);
    suspended.promise().root.promise().leaf = this->handle.promise().leaf;

    std::cerr << std::format(
        "setting {}'s parent to {}\n",
        handle.promise().functionName,
        suspended.promise().functionName);
    handle.promise().parent = suspended;
}

void CoroTask::update(float delta)
{
    std::cerr << "CoroTask::update\n";

    handle.promise().time += delta;

    auto leaf = handle.promise().leaf;
    if (leaf.done()) {
        handle.promise().leaf = leaf.promise().parent;
        std::cerr << "destroying coroutine: " << leaf.address() << "\n";
        leaf.destroy();
    }

    std::cerr << "resuming leaf: " << handle.promise().leaf.promise().functionName << "\n";
    handle.promise().leaf.resume();
}

CoroTask Promise::get_return_object()
{
    auto currentHandle = std::coroutine_handle<Promise>::from_promise(*this);

    //std::cerr << "created coroutine: " << currentHandle.address() << "\n";

    root = currentHandle;
    leaf = currentHandle;
    return CoroTask{.handle = currentHandle};
}

void Promise::unhandled_exception()
{
    exception = std::current_exception();
}

void Promise::return_void() const
{
}
