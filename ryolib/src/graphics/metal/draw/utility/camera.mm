#include "../private.h"

ryo::Matrix4 _compute_camera_vp(const RYCamera& camera, float aspect) {
    ryo::Matrix4 proj;
    switch (camera.projectionType) {
        case RYCameraProjectionTypeOrthographic: {
            float halfH = camera.orthographicSize * 0.5f;
            float halfW = halfH * aspect;
            proj = ryo::Matrix4::orthographic(-halfW, halfW, -halfH, halfH, camera.nearPlane, camera.farPlane);
            break;
        }
        case RYCameraProjectionTypePerspective: {
            proj = ryo::Matrix4::perspective(camera.fovY, aspect, camera.nearPlane, camera.farPlane);
            break;
        }
    }
    ryo::Matrix4 view = ryo::Matrix4::look_at(camera.position, camera.target, camera.up);
    return proj * view;
}
