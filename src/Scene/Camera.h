#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CG
{
    class Camera
    {
    public:
        Camera();
        ~Camera();

        auto GetProjectionMatrix() -> const glm::mat4
        {
            return projection;
        }

        auto GetViewMatrix() -> const glm::mat4
        {
            return view;
        }

        void SetFov(float degree)
        {
            fov = degree;

            UpdateProjectionMatrix();
        }

        void SetAspect(float ratio)
        {
            aspect = ratio;

            UpdateProjectionMatrix();
        }

        void SetClip(float near, float far)
        {
            clipNear = near;
            clipFar = far;

            UpdateProjectionMatrix();
        }

        /**
         * �� Yaw �M Pitch �[�W�Y�ӭ�
         * \param deltaYaw - yaw ���ܤƶq
         * \param deltaPitch - pitch ���ܤƶq
         */
        void changeYawPitch(float deltaYaw, float deltaPitch) {
            yaw += deltaYaw;
            pitch += deltaPitch;

            UpdateViewMatrix();
        }

        /**
         * �� distance �[�W�Y�ӭ�
         * \param deltaDistance - �ܤƶq
         */
        void changeDistance(float deltaDistance) {
            distance += deltaDistance;

            UpdateViewMatrix();
        }

    private:
        void UpdateProjectionMatrix();
        void UpdateViewMatrix();

    private:
        glm::mat4 projection;
        glm::mat4 view;

        float fov;
        float aspect;
        float clipNear;
        float clipFar;

        float yaw;  ///< �������ਤ�]���ר�^
        float pitch; ///< �]�����ਤ�]���ר�^
        float distance; ///< �����Z���ߦh��
        glm::vec3 center; ///< camera�`�������ߦ�m
    };
}
