#include "private.h"

#include "ryolib/math.h"

#include <cmath>

namespace ryo {
    Vector3 vector3_add(const Vector3& a, const Vector3& b) {
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    Vector3 vector3_subtract(const Vector3& a, const Vector3& b) {
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    Vector3 vector3_cross(const Vector3& a, const Vector3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    float vector3_dot(const Vector3& a, const Vector3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    float vector3_length(const Vector3& v) {
        return std::sqrt(vector3_dot(v, v));
    }

    Vector3 vector3_normalize(const Vector3& v) {
        float len = vector3_length(v);
        if (len == 0.0f) return {0.0f, 0.0f, 0.0f};
        return {v.x / len, v.y / len, v.z / len};
    }
};