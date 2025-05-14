#ifndef DT25_D_CAMERA_H
#define DT25_D_CAMERA_H

#include <memory>
#include <vector>
#include "../d_glob.h"
#include "d_NURBS.h"
#include <array>

class d_Camera {
    glm::vec3 d_rightVec;
    glm::vec3 d_upVec;
    glm::vec3 d_frontVec;
    glm::mat4 d_projectionMat;

    std::unique_ptr<d_NURBS> d_CameraCurve;

    std::vector<glm::vec3> d_CameraPoints;
    float d_CurrentCameraPathProgress = 0.0f;
    float d_CameraPathTargetTime;

    glm::vec3 d_GetCameraPointAt(float t);
    glm::vec3 d_GetCameraTraversedByDelta(float delta);
public:
    glm::vec3 d_cameraPos;

    float d_nearplane, d_farplane, d_fov, d_aspect;

    d_Camera(glm::vec3 _cameraPos, glm::vec3 _cameraTarget, glm::vec3 _upVec, std::vector<glm::vec3> _points, std::unique_ptr<d_NURBS> _curve, float _fov = 45.0f, float _nearplane = 0.1f, float _farplane = 1000.0f, float _targetTime=2.0f);

    bool d_UpdatePathPosition(float delta);    
    inline glm::mat4 d_getLookAt() { return glm::lookAt(d_cameraPos, d_cameraPos+d_frontVec + glm::vec3(0.0000001f, 0.0f, 0.0000001f), d_upVec); };
    inline glm::mat4 d_getProjectionMat() const { return d_projectionMat; }
    inline float d_GetProgress() const { return d_CurrentCameraPathProgress/d_CameraPathTargetTime; }        
    inline void d_Reset() { d_CurrentCameraPathProgress = 0; }

    std::array<glm::vec4, 6> d_getFrustum();
};


#endif
