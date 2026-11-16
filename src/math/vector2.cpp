#include "private.h"

#include "ryolib/math.h"

#include <cmath>

namespace ryo {
    Vector2 Vector2::operator+(const Vector2& b) const {
        return {
            x + b.x,
            y + b.y
        };
    }

    Vector2 Vector2::operator-(const Vector2& b) const {
        return {
            x - b.x,
            y - b.y
        };
    }

    Vector2 Vector2::operator*(float scalar) const {
        return {
            x * scalar,
            y * scalar
        };
    }

    Vector2 Vector2::operator/(float scalar) const {
        return {
            x / scalar,
            y / scalar
        };
    }
}