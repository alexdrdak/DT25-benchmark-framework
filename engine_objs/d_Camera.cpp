#include "d_Camera.h"


d_Camera::d_Camera(glm::vec3 _cameraPos, glm::vec3 _cameraTarget, glm::vec3 _upVec, std::vector<glm::vec3> _points, std::unique_ptr<d_NURBS> _curve, float _fov, float _nearplane, float _farplane, float _targetTime) {
    if (_cameraPos == _cameraTarget && !_curve)
    {
        d_Logger::err("Invalid camera configuration. Linear camera can't be initailized with equal start and end point.");
        throw std::invalid_argument("Invalid camera parameters...");
    }

    d_cameraPos = _cameraPos + glm::vec3(0.00000001f);
    d_frontVec = glm::normalize(_cameraTarget+glm::vec3(0.00000001f) - d_cameraPos);
    d_upVec = glm::vec3(0.0f, 1.0f, 0.0f);
    d_CameraCurve = std::move(_curve);
    d_fov = _fov;
    d_CameraPoints = _points;
    d_CameraPathTargetTime = _targetTime;

    d_aspect = 800.0f / 600.0f;
    d_projectionMat = glm::perspective(glm::radians(_fov), d_aspect, _nearplane, _farplane);

    d_nearplane = _nearplane;
    d_farplane = _farplane;
    d_fov = _fov;
}

glm::vec3 d_Camera::d_GetCameraPointAt(float t) {
    if (d_CameraCurve != nullptr)
        return d_CameraCurve->d_GetPointByPerc(t);
    else
        return d_CameraPoints[0] * (1 - t) + d_CameraPoints[1] * t;
}

glm::vec3 d_Camera::d_GetCameraTraversedByDelta(float delta) {
    d_CurrentCameraPathProgress = d_CurrentCameraPathProgress + (d_CurrentCameraPathProgress < d_CameraPathTargetTime ? delta : 0.0);
    return d_GetCameraPointAt(d_CurrentCameraPathProgress / d_CameraPathTargetTime);
}

bool d_Camera::d_UpdatePathPosition(float delta) {
    d_cameraPos = d_GetCameraTraversedByDelta(delta);  
    if(d_CameraCurve != nullptr)
        d_frontVec = glm::normalize(glm::vec3(0.0) - d_cameraPos);
    d_rightVec = glm::normalize(glm::cross(d_upVec, d_frontVec));

    return true;
}

std::array<glm::vec4, 6> d_Camera::d_getFrustum()
{
    glm::mat4 viewProj = d_getProjectionMat() * d_getLookAt();
    std::array<glm::vec4, 6> planes;

    planes[0] = glm::vec4(
        viewProj[0][3] + viewProj[0][0],
        viewProj[1][3] + viewProj[1][0],
        viewProj[2][3] + viewProj[2][0],
        viewProj[3][3] + viewProj[3][0]
    );

    planes[1] = glm::vec4(
        viewProj[0][3] - viewProj[0][0],
        viewProj[1][3] - viewProj[1][0],
        viewProj[2][3] - viewProj[2][0],
        viewProj[3][3] - viewProj[3][0]
    );

    planes[2] = glm::vec4(
        viewProj[0][3] + viewProj[0][1],
        viewProj[1][3] + viewProj[1][1],
        viewProj[2][3] + viewProj[2][1],
        viewProj[3][3] + viewProj[3][1]
    );

    planes[3] = glm::vec4(
        viewProj[0][3] - viewProj[0][1],
        viewProj[1][3] - viewProj[1][1],
        viewProj[2][3] - viewProj[2][1],
        viewProj[3][3] - viewProj[3][1]
    );

    planes[4] = glm::vec4(
        viewProj[0][2],
        viewProj[1][2],
        viewProj[2][2],
        viewProj[3][2]
    );

    planes[5] = glm::vec4(
        viewProj[0][3] - viewProj[0][2],
        viewProj[1][3] - viewProj[1][2],
        viewProj[2][3] - viewProj[2][2],
        viewProj[3][3] - viewProj[3][2]
    );

    for (int i = 0; i < 6; i++) {
        float len = glm::length(glm::vec3(planes[i]));
        planes[i] /= len;
    }

    return planes;
}
