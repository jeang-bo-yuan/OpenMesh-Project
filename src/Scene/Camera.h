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
         * 讓 Yaw 和 Pitch 加上某個值
         * \param deltaYaw - yaw 的變化量
         * \param deltaPitch - pitch 的變化量
         */
        void changeYawPitch(float deltaYaw, float deltaPitch) {
            yaw += deltaYaw;
            pitch += deltaPitch;

            UpdateViewMatrix();
        }

        /**
         * 讓 distance 加上某個值
         * \param deltaDistance - 變化量
         */
        void changeDistance(float deltaDistance) {
            distance += deltaDistance;

            UpdateViewMatrix();
        }

        /**
         * 改變中心的位置
         * \param translate - 平移
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

        glm::vec3 right_dir; ///< 右方
        glm::vec3 front_dir; ///< 前方（投影到xz平面）

        float fov;
        float aspect;
        float clipNear;
        float clipFar;

        float yaw;  ///< 水平旋轉角（弧度制）
        float pitch; ///< 鉛直旋轉角（弧度制）
        float distance; ///< 眼睛距中心多遠
        glm::vec3 center; ///< camera注視的中心位置
    };
}
