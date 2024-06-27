#pragma once

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