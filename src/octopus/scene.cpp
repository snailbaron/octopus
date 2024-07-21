#include "scene.hpp"

Object::Object(Sprite sprite, const WorldPosition& position)
    : _sprite(std::move(sprite))
    , _position(position)
    , _startTime(Clock::now())
    , _currentTime(_startTime)
{}

[[nodiscard]] const sdl::Texture& Object::texture() const
{
    return *_sprite.texture;
}

[[nodiscard]] sdl::Texture& Object::texture()
{
    return *_sprite.texture;
}

[[nodiscard]] const SDL_Rect& Object::frame() const
{
    auto framesPassed = (_currentTime - _startTime) / _sprite.frameDuration;
    auto frameIndex = framesPassed % _sprite.frames.size();
    return _sprite.frames.at(frameIndex);
}

void Object::update(float delta)
{
    _currentTime += std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<float>(delta));
}

[[nodiscard]] const WorldPosition& Object::position() const
{
    return _position;
}

void Object::moveTo(const WorldPosition& position)
{
    _position = position;
}

[[nodiscard]] ScreenPosition
Camera::project(const WorldPosition& worldPosition) const
{
    return ScreenPosition{
        .x = _viewport.center().x +
             (worldPosition.x - _center.x) * screenPixelsPerUnit(),
        .y = _viewport.center().y +
             (_center.y - worldPosition.y) * screenPixelsPerUnit(),
    };
}

[[nodiscard]] WorldPosition
Camera::restore(const ScreenPosition& screenPosition) const
{
    return WorldPosition{
        .x = _center.x +
             (screenPosition.x - _viewport.center().x) / screenPixelsPerUnit(),
        .y = _center.y +
             (_viewport.center().y - screenPosition.y) / screenPixelsPerUnit(),
    };
}

void Camera::viewport(int x, int y, int w, int h)
{
    _viewport = ScreenRect{
        .x = (float)x,
        .y = (float)y,
        .w = (float)w,
        .h = (float)h,
    };
}

void Camera::center(float x, float y)
{
    _center = WorldPosition{x, y};
}

void Camera::pixelsPerUnit(float pixelsPerUnit)
{
    _pixelsPerUnit = pixelsPerUnit;
}

void Camera::zoom(float zoom)
{
    _zoom = zoom;
}

[[nodiscard]] float Camera::zoom() const
{
    return _zoom;
}

[[nodiscard]] float Camera::screenPixelsPerUnit() const
{
    return _pixelsPerUnit * _zoom;
}

void Scene::addObject(size_t id, Sprite sprite, WorldPosition position)
{
    _objects[id] = Object{std::move(sprite), position};
}

void Scene::moveObject(size_t id, const WorldPosition& position)
{
    _objects.at(id).moveTo(position);
}

void Scene::killObject(size_t id)
{
    _objects.erase(id);
}

void Scene::update(float delta)
{
    for (auto& [id, object] : _objects) {
        object.update(delta);
    }
}

void Scene::render(sdl::Renderer& renderer)
{
    for (auto& [id, object] : _objects) {
        auto screenPosition = _camera.project(object.position());

        renderer.copy(
            object.texture(),
            object.frame(),
            SDL_FRect{
                .x = screenPosition.x -
                     _camera.zoom() * (float)object.frame().w / 2.f,
                .y = screenPosition.y -
                     _camera.zoom() * (float)object.frame().h / 2.f,
                .w = _camera.zoom() * (float)object.frame().w,
                .h = _camera.zoom() * (float)object.frame().h,
            });
    }
}

Camera& Scene::camera()
{
    return _camera;
}
