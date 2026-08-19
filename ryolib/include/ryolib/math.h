#pragma once

namespace ryo {
    struct Vector3 {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        Vector3 operator+(const Vector3& b) const;
        Vector3 operator-(const Vector3& b) const;
        Vector3 operator*(float scalar) const;
        Vector3 operator/(float scalar) const;
        Vector3 normalized() const;
        Vector3 cross(const Vector3& b) const;
        float dot(const Vector3& b) const;
        float length() const;
        float length_squared() const;
    };

    struct Matrix4 {
        float m[16];

        Matrix4();

        static Matrix4 perspective(float fovY, float aspect, float nearPlane, float farPlane);
        static Matrix4 orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
        static Matrix4 look_at(const Vector3& eye, const Vector3& target, const Vector3& up);
        static Matrix4 translation(float x, float y, float z);
        static Matrix4 scaling(float x, float y, float z);
        static Matrix4 rotation(float x, float y, float z);

        Matrix4 operator*(const Matrix4& b) const;
        Matrix4 inverse() const;
        Matrix4 transpose() const;
    };
}