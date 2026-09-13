#include "../private.h"

namespace ryo {
    Matrix4 _draw_compute_mvp(const InternalRenderer& renderer, const Matrix4& modelMatrix, const DrawSettings* settings) {
        if (settings && settings->camera) {
            int width, height;
            _draw_active_target_size_get(renderer, width, height);
            
            float aspect = static_cast<float>(width) / static_cast<float>(height);

            Matrix4 projection;
            if (settings->camera->projectionType == CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC) {
                float halfHeight = settings->camera->orthographicSize * 0.5f;
                float halfWidth = halfHeight * aspect;
                
                projection = Matrix4::orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, settings->camera->nearPlane, settings->camera->farPlane);
            } else {
                projection = Matrix4::perspective(settings->camera->fovY, aspect, settings->camera->nearPlane, settings->camera->farPlane);
            }

            Matrix4 view = Matrix4::look_at(settings->camera->position, settings->camera->target, settings->camera->up);
            return (projection * view * modelMatrix).transpose();
        }

        return modelMatrix.transpose();
    }
}