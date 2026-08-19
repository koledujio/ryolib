#include "private.h"

#include "ryolib/math.h"

#include <cmath>

namespace ryo {
    Matrix4::Matrix4() : m{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    } { }

    Matrix4 Matrix4::perspective(float fovY, float aspect, float nearPlane, float farPlane) { // logic/code borrowed from opengl's gluPerspective man page
        float deg2rad = M_PI / 180.0;
        float yScale = 1.0 / tan(deg2rad * fovY / 2);
        float xScale = yScale / aspect;
        float nearMinusFar = nearPlane - farPlane;

        Matrix4 m;
        m.m[0] = xScale;
        m.m[5] = yScale;
        m.m[10] = farPlane / nearMinusFar;
        m.m[11] = -1;
        m.m[14] = (farPlane * nearPlane) / nearMinusFar;
        m.m[15] = 0;

        return m;
    }

    Matrix4 Matrix4::orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
        Matrix4 m;
        m.m[0] = 2.0f / (right - left);
        m.m[5] = 2.0f / (top - bottom);
        m.m[10] = 1.0f / (farPlane - nearPlane);
        m.m[12] = -(right + left) / (right - left);
        m.m[13] = -(top + bottom) / (top - bottom);
        m.m[14] = -nearPlane / (farPlane - nearPlane);
        return m;
    }

    Matrix4 Matrix4::look_at(const Vector3& eye, const Vector3& target, const Vector3& up) {
        Vector3 f = eye - target;
        f = f.normalized();

        Vector3 l = up.cross(f);
        l = l.normalized();
        
        Vector3 u = f.cross(l);

        Matrix4 result;
        result.m[0] = l.x; result.m[4] = l.y; result.m[8] = l.z;
        result.m[1] = u.x; result.m[5] = u.y; result.m[9] = u.z;
        result.m[2] = f.x; result.m[6] = f.y; result.m[10] = f.z;

        result.m[12] = -l.dot(eye);
        result.m[13] = -u.dot(eye);
        result.m[14] = -f.dot(eye);

        return result;
    }

    Matrix4 Matrix4::translation(float x, float y, float z) {
        Matrix4 m;
        m.m[12] = x;
        m.m[13] = y;
        m.m[14] = z;
        return m;
    }

    Matrix4 Matrix4::scaling(float x, float y, float z) {
        Matrix4 m;
        m.m[0] = x;
        m.m[5] = y;
        m.m[10] = z;
        return m;
    }

    Matrix4 Matrix4::rotation(float x, float y, float z) {
        Matrix4 rx;
        rx.m[5] = std::cos(x);
        rx.m[6] = std::sin(x);
        rx.m[9] = -std::sin(x);
        rx.m[10] = std::cos(x);

        Matrix4 ry;
        ry.m[0] = std::cos(y);
        ry.m[2] = -std::sin(y);
        ry.m[8] = std::sin(y);
        ry.m[10] = std::cos(y);

        Matrix4 rz;
        rz.m[0] = std::cos(z);
        rz.m[1] = std::sin(z);
        rz.m[4] = -std::sin(z);
        rz.m[5] = std::cos(z);

        return rz * (ry * rx);
    }

    Matrix4 Matrix4::operator*(const Matrix4& b) const {
        Matrix4 result;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                float sum = 0.0f;
                for (int k = 0; k < 4; k++) {
                    sum += m[i + k * 4] * b.m[k + j * 4];
                }
                result.m[i + j*4] = sum;
            }
        }

        return result;
    }

    Matrix4 Matrix4::inverse() const {
        Matrix4 result;

        const float* a = m;
        float* inv = result.m;

        inv[0] =
            a[5]  * a[10] * a[15] -
            a[5]  * a[11] * a[14] -
            a[9]  * a[6]  * a[15] +
            a[9]  * a[7]  * a[14] +
            a[13] * a[6]  * a[11] -
            a[13] * a[7]  * a[10];

        inv[4] =
            -a[4] * a[10] * a[15] +
            a[4]  * a[11] * a[14] +
            a[8]  * a[6]  * a[15] -
            a[8]  * a[7]  * a[14] -
            a[12] * a[6]  * a[11] +
            a[12] * a[7]  * a[10];

        inv[8] =
            a[4]  * a[9] * a[15] -
            a[4]  * a[11] * a[13] -
            a[8]  * a[5] * a[15] +
            a[8]  * a[7] * a[13] +
            a[12] * a[5] * a[11] -
            a[12] * a[7] * a[9];

        inv[12] =
            -a[4] * a[9] * a[14] +
            a[4]  * a[10] * a[13] +
            a[8]  * a[5] * a[14] -
            a[8]  * a[6] * a[13] -
            a[12] * a[5] * a[10] +
            a[12] * a[6] * a[9];

        inv[1] =
            -a[1] * a[10] * a[15] +
            a[1]  * a[11] * a[14] +
            a[9]  * a[2] * a[15] -
            a[9]  * a[3] * a[14] -
            a[13] * a[2] * a[11] +
            a[13] * a[3] * a[10];

        inv[5] =
            a[0]  * a[10] * a[15] -
            a[0]  * a[11] * a[14] -
            a[8]  * a[2] * a[15] +
            a[8]  * a[3] * a[14] +
            a[12] * a[2] * a[11] -
            a[12] * a[3] * a[10];

        inv[9] =
            -a[0] * a[9] * a[15] +
            a[0]  * a[11] * a[13] +
            a[8]  * a[1] * a[15] -
            a[8]  * a[3] * a[13] -
            a[12] * a[1] * a[11] +
            a[12] * a[3] * a[9];

        inv[13] =
            a[0]  * a[9] * a[14] -
            a[0]  * a[10] * a[13] -
            a[8]  * a[1] * a[14] +
            a[8]  * a[2] * a[13] +
            a[12] * a[1] * a[10] -
            a[12] * a[2] * a[9];

        inv[2] =
            a[1]  * a[6] * a[15] -
            a[1]  * a[7] * a[14] -
            a[5]  * a[2] * a[15] +
            a[5]  * a[3] * a[14] +
            a[13] * a[2] * a[7] -
            a[13] * a[3] * a[6];

        inv[6] =
            -a[0] * a[6] * a[15] +
            a[0]  * a[7] * a[14] +
            a[4]  * a[2] * a[15] -
            a[4]  * a[3] * a[14] -
            a[12] * a[2] * a[7] +
            a[12] * a[3] * a[6];

        inv[10] =
            a[0]  * a[5] * a[15] -
            a[0]  * a[7] * a[13] -
            a[4]  * a[1] * a[15] +
            a[4]  * a[3] * a[13] +
            a[12] * a[1] * a[7] -
            a[12] * a[3] * a[5];

        inv[14] =
            -a[0] * a[5] * a[14] +
            a[0]  * a[6] * a[13] +
            a[4]  * a[1] * a[14] -
            a[4]  * a[2] * a[13] -
            a[12] * a[1] * a[6] +
            a[12] * a[2] * a[5];

        inv[3] =
            -a[1] * a[6] * a[11] +
            a[1]  * a[7] * a[10] +
            a[5]  * a[2] * a[11] -
            a[5]  * a[3] * a[10] -
            a[9]  * a[2] * a[7] +
            a[9]  * a[3] * a[6];

        inv[7] =
            a[0]  * a[6] * a[11] -
            a[0]  * a[7] * a[10] -
            a[4]  * a[2] * a[11] +
            a[4]  * a[3] * a[10] +
            a[8]  * a[2] * a[7] -
            a[8]  * a[3] * a[6];

        inv[11] =
            -a[0] * a[5] * a[11] +
            a[0]  * a[7] * a[9] +
            a[4]  * a[1] * a[11] -
            a[4]  * a[3] * a[9] -
            a[8]  * a[1] * a[7] +
            a[8]  * a[3] * a[5];

        inv[15] =
            a[0]  * a[5] * a[10] -
            a[0]  * a[6] * a[9] -
            a[4]  * a[1] * a[10] +
            a[4]  * a[2] * a[9] +
            a[8]  * a[1] * a[6] -
            a[8]  * a[2] * a[5];

        float determinant =
            a[0] * inv[0] +
            a[1] * inv[4] +
            a[2] * inv[8] +
            a[3] * inv[12];

        if (determinant == 0.0f)
            return Matrix4();

        determinant = 1.0f / determinant;

        for (int i = 0; i < 16; ++i)
            inv[i] *= determinant;

        return result;
    }

    Matrix4 Matrix4::transpose() const {
        Matrix4 result;

        result.m[0]  = m[0];
        result.m[1]  = m[4];
        result.m[2]  = m[8];
        result.m[3]  = m[12];

        result.m[4]  = m[1];
        result.m[5]  = m[5];
        result.m[6]  = m[9];
        result.m[7]  = m[13];

        result.m[8]  = m[2];
        result.m[9]  = m[6];
        result.m[10] = m[10];
        result.m[11] = m[14];

        result.m[12] = m[3];
        result.m[13] = m[7];
        result.m[14] = m[11];
        result.m[15] = m[15];

        return result;
    }
}