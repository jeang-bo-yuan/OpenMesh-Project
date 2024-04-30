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

        const glm::vec3 GetRight() { return right_dir; }
        const glm::vec3 GetFront() { return front_dir; }

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

        /**
         * ���ܤ��ߪ���m
         * \param translate - ����
         */
        void changeCenter(glm::vec3 translate) {
            center += translate;

            UpdateViewMatrix();
        }

    private:
        void UpdateProjectionMatrix();
        void UpdateViewMatrix();

    private:
        glm::mat4 projection;
        glm::mat4 view;

        glm::vec3 right_dir; ///< �k��
        glm::vec3 front_dir; ///< �e��]��v��xz�����^

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
