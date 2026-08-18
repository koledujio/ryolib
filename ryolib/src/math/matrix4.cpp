#include "private.h"

#include "ryolib/math.h"

#include <cmath>

namespace ryo {
    Matrix4 matrix4_identity() {
        Matrix4 m;

        m.m[0] = 1;
        m.m[1] = 0;
        m.m[2] = 0;
        m.m[3] = 0;

        m.m[4] = 0;
        m.m[5] = 1;
        m.m[6] = 0;
        m.m[7] = 0;

        m.m[8] = 0;
        m.m[9] = 0;
        m.m[10] = 1;
        m.m[11] = 0;

        m.m[12] = 0;
        m.m[13] = 0;
        m.m[14] = 0;
        m.m[15] = 1;

        return m;
    }

    Matrix4 matrix4_perspective(float fovY, float aspect, float nearPlane, float farPlane) { // logic/code borrowed from opengl's gluPerspective man page
        float deg2rad = M_PI / 180.0;
        float yScale = 1.0 / tan(deg2rad * fovY / 2);
        float xScale = yScale / aspect;
        float nearMinusFar = nearPlane - farPlane;

        return {
            xScale, 0, 0, 0,
            0, yScale, 0, 0,
            0, 0, farPlane / nearMinusFar, -1,
            0, 0, (farPlane*nearPlane) / nearMinusFar, 0
        };
    }

    Matrix4 matrix4_orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
        Matrix4 m = matrix4_identity();
        m.m[0] = 2.0f / (right - left);
        m.m[5] = 2.0f / (top - bottom);
        m.m[10] = 1.0f / (farPlane - nearPlane);
        m.m[12] = -(right + left) / (right - left);
        m.m[13] = -(top + bottom) / (top - bottom);
        m.m[14] = -nearPlane / (farPlane - nearPlane);
        return m;
    }

    Matrix4 matrix4_look_at(const Vector3& eye, const Vector3& target, const Vector3& up) {
        Vector3 f = vector3_normalize(vector3_subtract(eye, target));
        Vector3 l = vector3_normalize(vector3_cross(up, f));
        Vector3 u = vector3_cross(f, l);

        Matrix4 result = matrix4_identity();
        result.m[0] = l.x; result.m[4] = l.y; result.m[8] = l.z;
        result.m[1] = u.x; result.m[5] = u.y; result.m[9] = u.z;
        result.m[2] = f.x; result.m[6] = f.y; result.m[10] = f.z;

        result.m[12] = -vector3_dot(l, eye);
        result.m[13] = -vector3_dot(u, eye);
        result.m[14] = -vector3_dot(f, eye);

        return result;
    }

    Matrix4 matrix4_multiply(const Matrix4& a, const Matrix4& b) {
        Matrix4 result;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                float sum = 0.0f;
                for (int k = 0; k < 4; k++) {
                    sum += a.m[i + k * 4] * b.m[k + j * 4];
                }
                result.m[i + j*4] = sum;
            }
        }

        return result;
    }

    Matrix4 matrix4_translation(float x, float y, float z) {
        Matrix4 m = matrix4_identity();
        m.m[12] = x;
        m.m[13] = y;
        m.m[14] = z;
        return m;
    }

    Matrix4 matrix4_scaling(float x, float y, float z) {
        Matrix4 m = matrix4_identity();
        m.m[0] = x;
        m.m[5] = y;
        m.m[10] = z;
        return m;
    }

    Matrix4 matrix4_rotation(float x, float y, float z) {
        Matrix4 rx = matrix4_identity();
        rx.m[5] = std::cos(x);
        rx.m[6] = std::sin(x);
        rx.m[9] = -std::sin(x);
        rx.m[10] = std::cos(x);

        Matrix4 ry = matrix4_identity();
        ry.m[0] = std::cos(y);
        ry.m[2] = -std::sin(y);
        ry.m[8] = std::sin(y);
        ry.m[10] = std::cos(y);

        Matrix4 rz = matrix4_identity();
        rz.m[0] = std::cos(z);
        rz.m[1] = std::sin(z);
        rz.m[4] = -std::sin(z);
        rz.m[5] = std::cos(z);

        return matrix4_multiply(rz, matrix4_multiply(ry, rx));
    }
}