#include "../private.h"

namespace ryo {
    Matrix4 _draw_compute_camera_vp(const Camera& camera, float aspect) {
        Matrix4 projection;
        if (camera.projectionType == CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC) {
            float halfHeight = camera.orthographicSize * 0.5f;
            float halfWidth = halfHeight * aspect;
            projection = Matrix4::orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, camera.nearPlane, camera.farPlane);
        } else {
            projection = Matrix4::perspective(camera.fovY, aspect, camera.nearPlane, camera.farPlane);
        }

        Matrix4 view = Matrix4::look_at(camera.position, camera.target, camera.up);
        return projection * view;
    }

    Matrix4 _draw_compute_mvp(const InternalRenderer& renderer, const Matrix4& modelMatrix, const DrawSettings* settings) {
        if (settings && settings->camera) {
            int width, height;
            _draw_get_target_size(renderer, width, height);
            
            float aspect = static_cast<float>(width) / static_cast<float>(height);
            Matrix4 cameraVp = _draw_compute_camera_vp(*settings->camera, aspect);
            return (cameraVp * modelMatrix).transpose();
        }

        return modelMatrix.transpose();
    }
}