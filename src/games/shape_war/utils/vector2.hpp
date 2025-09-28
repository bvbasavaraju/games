#pragma once

// SFML headers
#include <SFML/Graphics.hpp>

// std headers
#include <math.h>
#include <type_traits>

namespace bv::lab {

template <typename T>
concept ArithmeticType_t = std::is_arithmetic_v<T>;

template <ArithmeticType_t T>
struct Vector_t {
    T x{};
    T y{};

    Vector_t() = default;
    
    Vector_t(T x_, T y_)
    : x(x_), y(y_) {}

    Vector_t(sf::Vector2<T> const& sfVec) 
    : x(sfVec.x), y(sfVec.y) {}

    operator sf::Vector2<T>() const {
        return sf::Vector2<T>(x, y);
    }

    auto operator+(Vector_t const& rhs) const {
        return Vector_t{x+rhs.x, y+rhs.y};
    }

    auto operator-(Vector_t const& rhs) const {
        return Vector_t{x-rhs.x, y-rhs.y};
    }

    auto operator*(Vector_t const& rhs) const {
        return Vector_t{x*rhs.x, y*rhs.y};
    }

    auto operator/(Vector_t const& rhs) const {
        // TODO validate for 0
        return Vector_t{x/rhs.x, y/rhs.y};
    }

    void operator+=(Vector_t const& rhs) {
        x += rhs.x;
        y += rhs.y;
    }

    auto operator-=(Vector_t const& rhs) {
        x -= rhs.x;
        y -= rhs.y;
    }

    auto operator*=(Vector_t const& rhs) {
        x *= rhs.x;
        y *= rhs.y;
    }

    auto operator/=(Vector_t const& rhs) {
        // TODO validate for 0
        x /= rhs.x;
        y /= rhs.y;
    }

    auto operator==(Vector_t const& other) const {
        return other.x == x && other.y == y; 
    }

    auto operator!=(Vector_t const& other) const{
        return !(*this == other);
    }

    float distance(Vector_t const& other) const {
        auto nv = *this - other; 
        return sqrtf((nv.x * nv.x) + (nv.y * nv.y));
    }

    float length() const {
        Vector_t<T> origin = {0, 0};
        return distance(origin);
    }

    void normalize() {
        auto len = length();
        x /= len;
        y /= len;
    }

    float angle() const {
        return atan2f(y, x);
    }

    static auto getCoOrdinates(float speed, float angle_) {
        auto angleInRadians = angle_ * 3.14f / 180.0f;
        return Vector_t{speed * cos(angleInRadians), speed * sin(angleInRadians)};
    }
};

using Vec2f = Vector_t<float>;
using Vec2i = Vector_t<int>;

}   // namespace bv::lab (game by BV)
