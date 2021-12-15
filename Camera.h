#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const glm::vec3 POS = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

//Default Projection Values
const float FOV = glm::radians(60.0f);
const float WIDTH = 800.0f;
const float HEIGHT = 600.0f;
const float NEAR = 0.1f;
const float FAR = 100.0f;

/*
Class : Camera
This class was made to manipulate the camera in the OpenGL scene
It creates the view and projection matrix according to user preferences
*/
class Camera
{
private:
    //Float Parameters
    float yaw;
    float pitch;
    float zoom; //Still need to be coded

    //Vector Parameters
    glm::vec3 worldUp;
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;

    glm::mat4 view;
    glm::mat4 projection;

public:
    Camera();

    //Access methods
    glm::mat4 getProjectionMat() const;
    glm::mat4 getView() const;
    glm::vec3 getFront() const;
    glm::vec3 getUp() const;
    glm::vec3 getPos() const;
    glm::vec3 getRight() const;
    float getYaw() const;
    float getPitch() const;

    //Setting methods
    void setPosition(glm::vec3 newPosition);
    void setPosition(float newX, float newY, float newZ);
    void setYaw(float newYaw);
    void setPitch(float newPitch);
    void setYP(float newYaw, float newPitch);

    void setPerspectiveProjection(float newFov, float newWidth, float newHeight, float newNear, float newFar);
    void updateCameraMatrix();


private:
    void updateCameraVectors();
};