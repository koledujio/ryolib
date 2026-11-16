#include "private.h"

#include "ryolib/math.h"

#include <cmath>

namespace ryo {
    Vector3 Vector3::operator+(const Vector3& b) const {
        return {
            x + b.x,
            y + b.y,
            z + b.z
        };
    }

    Vector3 Vector3::operator-(const Vector3& b) const {
        return {
            x - b.x,
            y - b.y,
            z - b.z
        };
    }

    Vector3 Vector3::operator*(float scalar) const {
        return {
            x * scalar,
            y * scalar,
            z * scalar
        };
    }

    Vector3 Vector3::operator/(float scalar) const {
        return {
            x / scalar,
            y / scalar,
            z / scalar
        };
    }

    Vector3 Vector3::normalized() const {
        float len = length();
        if (len == 0.0f) return {0.0f, 0.0f, 0.0f};

        return {
            x / len,
            y / len,
            z / len
        };
    }

    Vector3 Vector3::cross(const Vector3& b) const {
        return {
            y * b.z - z * b.y,
            z * b.x - x * b.z,
            x * b.y - y * b.x
        };
    }

    float Vector3::dot(const Vector3& b) const {
        return x * b.x + y * b.y + z * b.z;
    }

    float Vector3::length() const {
        return std::sqrt(length_squared());
    }

    float Vector3::length_squared() const {
        return dot(*this);
    }
}