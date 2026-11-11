#include "../private.h"

namespace ryo {
    Matrix4 _compute_camera_vp(const Camera& camera, float aspect) {
        Matrix4 proj;
        switch (camera.projectionType) {
            case CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC: {
                float halfH = camera.orthographicSize * 0.5f;
                float halfW = halfH * aspect;
                proj = Matrix4::orthographic(-halfW, halfW, -halfH, halfH, camera.nearPlane, camera.farPlane);
                break;
            }
            case CAMERA_PROJECTION_TYPE_PERSPECTIVE: {
                proj = Matrix4::perspective(camera.fovY, aspect, camera.nearPlane, camera.farPlane);
                break;
            }
        }
        Matrix4 view = Matrix4::look_at(camera.position, camera.target, camera.up);
        return proj * view;
    }
}