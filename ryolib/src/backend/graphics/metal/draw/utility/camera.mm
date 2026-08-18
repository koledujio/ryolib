#include "../private.h"

namespace ryo {
    Matrix4 _compute_camera_vp(const Camera& camera, float aspect) {
        Matrix4 proj;
        switch (camera.projectionType) {
            case CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC: {
                float halfH = camera.orthographicSize * 0.5f;
                float halfW = halfH * aspect;
                proj = matrix4_orthographic(-halfW, halfW, -halfH, halfH, camera.nearPlane, camera.farPlane);
                break;
            }
            case CAMERA_PROJECTION_TYPE_PERSPECTIVE: {
                proj = matrix4_perspective(camera.fovY, aspect, camera.nearPlane, camera.farPlane);
                break;
            }
        }
        Matrix4 view = matrix4_look_at(camera.position, camera.target, camera.up);
        return matrix4_multiply(proj, view);
    }
}