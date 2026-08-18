#pragma once

namespace ryo {
    struct Matrix4 {
        float m[16];
    };

    struct Vector3 {
        float x, y, z;
    };

    Matrix4 matrix4_identity();
    Matrix4 matrix4_perspective(float fovY, float aspect, float nearPlane, float farPlane);
    Matrix4 matrix4_orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    Matrix4 matrix4_look_at(const Vector3& eye, const Vector3& target, const Vector3& up);
    Matrix4 matrix4_multiply(const Matrix4& a, const Matrix4& b);
    Matrix4 matrix4_translation(float x, float y, float z);
    Matrix4 matrix4_scaling(float x, float y, float z);
    Matrix4 matrix4_rotation(float x, float y, float z);

    Vector3 vector3_add(const Vector3& a, const Vector3& b);
    Vector3 vector3_subtract(const Vector3& a, const Vector3& b);
    Vector3 vector3_cross(const Vector3& a, const Vector3& b);
    float vector3_dot(const Vector3& a, const Vector3& b);
    float vector3_length(const Vector3& v);
    Vector3 vector3_normalize(const Vector3& v);
}