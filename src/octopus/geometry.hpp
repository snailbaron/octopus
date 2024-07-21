#pragma once

#include <cmath>
#include <format>
#include <ostream>
#include <stdexcept>

template <class T, class Tag = void>
class Norm {
public:
    Norm(T x, T y)
    {
        auto l = std::sqrt(x * x + y * y);
        if (l == 0) {
            throw std::runtime_error{std::format(
                "Norm: provided vector ({}, {}) has zero length", x, y)};
        }

        _x = x / l;
        _y = y / l;
    }

    const T& x() const
    {
        return _x;
    }

    const T& y() const
    {
        return _y;
    }

private:
    T _x {};
    T _y {};
};

template <class T, class Tag>
std::ostream& operator<<(std::ostream& out, const Norm<T, Tag>& norm)
{
    return out << std::format("({}, {})", norm.x(), norm.y());
}

template <class T, class Tag = void>
struct Vector {
    Vector& operator+=(const Vector& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector& operator-=(const Vector& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector& operator*=(const T& scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector& operator/=(const T& scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    T sqLength() const
    {
        return x * x + y * y;
    }

    T length() const
    {
        return std::sqrt(sqLength());
    }

    Norm<T, Tag> norm() const
    {
        return {x, y};
    }

    T x {};
    T y {};
};

template <class T, class Tag>
Vector<T, Tag> operator+(Vector<T, Tag> lhs, const Vector<T, Tag>& rhs)
{
    lhs += rhs;
    return lhs;
}

template <class T, class Tag>
Vector<T, Tag> operator-(Vector<T, Tag> lhs, const Vector<T, Tag>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template <class T, class Tag>
Vector<T, Tag> operator*(Vector<T, Tag> vector, const T& scalar)
{
    vector *= scalar;
    return vector;
}

template <class T, class Tag>
Vector<T, Tag> operator*(const T& scalar, Vector<T, Tag> vector)
{
    vector *= scalar;
    return vector;
}

template <class T, class Tag>
Vector<T, Tag> operator/(Vector<T, Tag> vector, const T& scalar)
{
    vector /= scalar;
    return vector;
}

template <class T, class Tag>
Vector<T, Tag> operator*(const Norm<T, Tag>& norm, const T& scalar)
{
    return {norm.x() * scalar, norm.y() * scalar};
}

template <class T, class Tag>
Vector<T, Tag> operator*(const T& scalar, const Norm<T, Tag>& norm)
{
    return {norm.x() * scalar, norm.y() * scalar};
}

template <class T, class Tag>
Vector<T, Tag> operator/(const Norm<T, Tag>& norm, const T& scalar)
{
    return {norm.x() / scalar, norm.y() / scalar};
}

template <class T, class Tag>
std::ostream& operator<<(std::ostream& out, const Vector<T, Tag>& vector)
{
    return out << std::format("({}, {})", vector.x, vector.y);
}

template <class T, class Tag = void>
struct Point {
    Point& operator+=(const Vector<T, Tag>& vector)
    {
        x += vector.x;
        y += vector.y;
        return *this;
    }

    Point& operator-=(const Vector<T, Tag>& vector)
    {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }

    T x {};
    T y {};
};

template <class T, class Tag>
std::ostream& operator<<(std::ostream& out, const Point<T, Tag>& point)
{
    return out << std::format("({}, {})", point.x, point.y);
}

template <class T, class Tag>
Point<T, Tag> operator+(Point<T, Tag> point, const Vector<T, Tag>& vector)
{
    point += vector;
    return point;
}

template <class T, class Tag>
Point<T, Tag> operator-(Point<T, Tag> point, const Vector<T, Tag>& vector)
{
    point -= vector;
    return point;
}

template <class T, class Tag>
Vector<T, Tag> operator-(const Point<T, Tag>& lhs, const Point<T, Tag>& rhs)
{
    return Vector<T, Tag>{lhs.x - rhs.x, lhs.y - rhs.y};
}

template <class T, class Tag>
struct Rect {
    Point<T, Tag> center() const
    {
        return {x + w / 2, y + h / 2};
    }

    T x {};
    T y {};
    T w {};
    T h {};
};

template <class T, class Tag>
T distance(const Point<T, Tag>& lhs, const Point<T, Tag>& rhs)
{
    return (rhs - lhs).length();
}


struct WorldTag;
struct ScreenTag;

using WorldPosition = Point<float, WorldTag>;
using WorldVector = Vector<float, WorldTag>;

using ScreenPosition = Point<float, ScreenTag>;
using ScreenRect = Rect<float, ScreenTag>;