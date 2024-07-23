#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <queue>
#include <span>
#include <typeindex>
#include <utility>
#include <vector>

class Entity {
public:
    using ValueType = uint32_t;

    Entity() = default;

    explicit constexpr Entity(ValueType id)
        : _id(id)
    { }

    constexpr operator ValueType() const
    {
        return _id;
    }

    constexpr auto operator<=>(const Entity&) const = default;

private:
    ValueType _id = 0;
};

class EntityPool {
public:
    Entity create()
    {
        if (!_discardedIds.empty()) {
            auto entity = Entity{_discardedIds.front()};
            _discardedIds.pop();
            return entity;
        }
        return Entity{_nextId++};
    }

    void kill(Entity entity)
    {
        _discardedIds.push(entity);
    }

private:
    Entity::ValueType _nextId = 0;
    std::queue<Entity> _discardedIds;
};

class AbstractComponentStorage {
public:
    virtual ~AbstractComponentStorage() = default;

    virtual void kill(Entity entity) = 0;
};

template <class Component>
class ComponentStorage : public AbstractComponentStorage {
public:
    Component& component(Entity entity)
    {
        return _components.at(_componentIndexByEntity.at(entity));
    }

    const Component& component(Entity entity) const
    {
        return _components.at(_componentIndexByEntity.at(entity));
    }

    std::span<Component> components()
    {
        return _components;
    }

    std::span<const Component> components() const
    {
        return _components;
    }

    std::span<const Entity> entities() const
    {
        return _entities;
    }

    Component& add(Entity entity, const Component& component)
    {
        _componentIndexByEntity[entity] = _components.size();
        _entities.push_back(entity);
        _components.push_back(component);
        return _components.back();
    }

    Component& add(Entity entity, Component&& component)
    {
        _componentIndexByEntity[entity] = _components.size();
        _entities.push_back(entity);
        _components.push_back(std::move(component));
        return _components.back();
    }

    template <class... Args>
    Component& emplace(Entity entity, Args&&... args)
    {
        _componentIndexByEntity[entity] = _components.size();
        _entities.push_back(entity);
        return _components.emplace_back(std::forward<Args>(args)...);
    }

    void kill(Entity entity) override
    {
        auto index = _componentIndexByEntity.at(entity);
        if (index + 1 < _entities.size()) {
            std::swap(_entities.at(index), _entities.back());
            std::swap(_components.at(index), _components.back());
            auto movedEntity = _entities.at(index);
            _componentIndexByEntity.at(movedEntity) = index;
        }
        _entities.resize(_entities.size() - 1);
        _components.resize(_entities.size() - 1);
        _componentIndexByEntity.erase(entity);
    }

private:
    std::vector<Entity> _entities;
    std::vector<Component> _components;
    std::map<Entity, size_t> _componentIndexByEntity;
};

class Ecs {
public:
    template <class Component>
    Component& component(Entity entity)
    {
        return existingStorage<Component>().component(entity);
    }

    template <class Component>
    const Component& component(Entity entity) const
    {
        return existingStorage<Component>().component(entity);
    }

    template <class Component>
    std::span<Component> components()
    {
        return existingStorage<Component>().components();
    }

    template <class Component>
    std::span<const Component> components() const
    {
        return existingStorage<Component>().components();
    }

    template <class Component>
    std::span<const Entity> entities() const
    {
        return existingStorage<Component>().entities();
    }

    template <class Component>
    Component& add(Entity entity, Component&& component)
    {
        _entityComponentTypes[entity].push_back(typeid(Component));
        return storage<Component>().add(
            entity, std::forward<Component>(component));
    }

    template <class Component, class... Args>
    Component& emplace(Entity entity, Args&&... args)
    {
        _entityComponentTypes[entity].push_back(typeid(Component));
        return storage<Component>().emplace(
            entity, std::forward<Args>(args)...);
    }

    Entity create()
    {
        return _entityPool.create();
    }

    void kill(Entity entity)
    {
        for (const auto& typeIndex : _entityComponentTypes.at(entity)) {
            _storages.at(typeIndex)->kill(entity);
        }
        _entityComponentTypes.erase(entity);
        _entityPool.kill(entity);
    }

private:
    template <class Component>
    ComponentStorage<Component>& existingStorage()
    {
        return static_cast<ComponentStorage<Component>&>(
            *_storages.at(typeid(Component)));
    }

    template <class Component>
    const ComponentStorage<Component>& existingStorage() const
    {
        return static_cast<const ComponentStorage<Component>&>(
            *_storages.at(typeid(Component)));
    }

    template <class Component>
    ComponentStorage<Component>& storage()
    {
        auto [it, inserted] = _storages.emplace(typeid(Component), nullptr);
        if (inserted) {
            it->second = std::make_unique<ComponentStorage<Component>>();
        }
        return static_cast<ComponentStorage<Component>&>(*it->second);
    }

    EntityPool _entityPool;
    std::map<std::type_index, std::unique_ptr<AbstractComponentStorage>> _storages;
    std::map<Entity, std::vector<std::type_index>> _entityComponentTypes;
};