#pragma once

#include <any>
#include <concepts>
#include <functional>
#include <map>
#include <memory>
#include <typeindex>
#include <vector>

class LifeTracker {
public:
    LifeTracker() = default;

    explicit operator bool() const noexcept
    {
        return !_ptr.expired();
    }

private:
    explicit LifeTracker(const std::shared_ptr<char>& holder) noexcept
        : _ptr(holder)
    { }

    std::weak_ptr<char> _ptr;

    friend class LifeHolder;
};

class LifeHolder {
public:
    LifeTracker tracker() const noexcept
    {
        return LifeTracker{_ptr};
    }

private:
    std::shared_ptr<char> _ptr = std::make_shared<char>();
};

class Channel {
public:
    template <class Event>
    void push(Event&& event)
    {
        events<Event>().push_back(std::forward<Event>(event));
    }

    template <class Event>
    void emplace(Event&& event)
    {
        events<Event>().emplace_back(std::forward<Event>(event));
    }

    template <class Event, std::invocable<const Event&> Handler>
    void subscribe(Handler&& handler, const LifeHolder& lifeHolder)
    {
        auto typeIndex = std::type_index{typeid(Event)};

        _subscriberMap[typeIndex].push_back(Subscription{
            .tracker = lifeHolder.tracker(),
            .handler = [handler](const std::any& event) {
                handler(std::any_cast<const Event&>(event));
            },
        });
    }

    void deliver()
    {
        for (size_t i = 0; i < _events.size(); i++) {
            const auto& event = _events.at(i);
            const auto& typeInfo = _typeInfos.at(i);
            auto typeIndex = std::type_index{typeInfo};

            auto& subs = _subscriberMap.at(typeIndex);
            for (size_t j = 0; j < subs.size();) {
                if (subs.at(j).tracker) {
                    subs.at(j).handler(event);
                    j++;
                } else {
                    if (j + 1 < subs.size()) {
                        std::swap(subs.at(j), subs.back());
                    }
                    subs.resize(subs.size() - 1);
                }
            }
        }
    }

private:
    struct Subscription {
        LifeTracker tracker;
        std::function<void(const std::any& event)> handler;
    };

    std::vector<std::any> _events;
    std::vector<std::type_info> _typeInfos;
    std::map<std::type_index, std::vector<Subscription>> _subscriberMap;
};

class Subscriber {
public:
    template <class Event, std::invocable<const Event&> Handler>
    void subscribe(Channel& channel, Handler&& handler)
    {
        channel.subscribe(std::forward<Handler>(handler), _lifeTracker);
    }

private:
    LifeHolder _lifeTracker;
};