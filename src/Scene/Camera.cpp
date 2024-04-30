#include "Camera.h"

#include <iostream>
#include <algorithm>

namespace CG
{
    Camera::Camera()
    {
        projection = glm::mat4(1.0);
        view = glm::mat4(1.0);

        fov = 80.0f;
        aspect = 4.0f / 3.0f;
        clipNear = 0.01f;
        clipFar = 1000.0f;

        yaw = glm::pi<float>() / 2.f;
        pitch = 0.f;
        center = glm::vec3(0, 0, 0);
        distance = 1.f;

        UpdateProjectionMatrix();
        UpdateViewMatrix();
    }

    Camera::~Camera()
    {

    }

    void Camera::UpdateProjectionMatrix()
    {
        projection = glm::perspective(glm::radians(fov), aspect, clipNear, clipFar);
    }

    void Camera::UpdateViewMatrix()
    {
        // pitch ����W�X ���tpi/2���d��
        pitch = glm::clamp(pitch, -glm::pi<float>() / 2.f + 0.00001f, glm::pi<float>() / 2.f - 0.00001f);
        // distance ����p�󵥩�0
        distance = std::max(0.001f, distance);

        //std::cout << "(yaw, pitch) = (" << yaw << ", " << pitch << ")" << std::endl;

        // �q�����I���V�۾����V�q
        glm::vec3 dir(
            distance * cosf(pitch) * cosf(yaw),
            distance * sinf(pitch),
            distance * cosf(pitch) * sinf(yaw)
        );
        view = glm::lookAt(center + dir, center, glm::vec3(0, 1, 0));
    }
}