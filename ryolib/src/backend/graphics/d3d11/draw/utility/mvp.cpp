#include "../private.h"

#include "ryolib/math.h"

namespace ryo {
    Matrix4 _draw_compute_camera_vp(const Camera& camera, float aspect) {
        Matrix4 projection;

        switch (camera.projectionType) {
            case CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC: {
                float halfHeight = camera.orthographicSize * 0.5f;
                float halfWidth = halfHeight * aspect;

                projection = Matrix4::orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, camera.nearPlane, camera.farPlane);
                break;
            }

            case CAMERA_PROJECTION_TYPE_PERSPECTIVE: {
                projection = Matrix4::perspective(camera.fovY, aspect, camera.nearPlane, camera.farPlane);
                break;
            }
        }

        Matrix4 view = Matrix4::look_at(camera.position, camera.target, camera.up);

        return projection * view;
    }

    Matrix4 _draw_get_mvp(const InternalRenderer& renderer, const DrawSettings* settings, const Matrix4& matrix, int targetWidth, int targetHeight) {
        if (!settings || !settings->camera) return matrix;

        float aspect = static_cast<float>(targetWidth) / static_cast<float>(targetHeight);

        Matrix4 cameraMatrix = _draw_compute_camera_vp(*settings->camera, aspect);

        return cameraMatrix * matrix;
    }

    bool _draw_bind_mvp(InternalRenderer& renderer, const Matrix4& mvp) {
        ID3D11Buffer* buffer = nullptr;

        if (!_draw_create_dynamic_buffer(renderer.device, &mvp, sizeof(Matrix4), D3D11_BIND_CONSTANT_BUFFER, &buffer))
            return false;

        renderer.context->VSSetConstantBuffers(1, 1, &buffer);

        buffer->Release();
        return true;
    }
}

