#include "FPVCallbacks.h"

float FPVCallbacks::yaw = YAW;
float FPVCallbacks::pitch = PITCH;
bool FPVCallbacks::firstMouse = true;
float FPVCallbacks::lastX = 400;
float FPVCallbacks::lastY = 300;
float FPVCallbacks::sensitivity = 0.1;

glm::vec3 FPVCallbacks::camPos = glm::vec3(0.0f, 0.0f, 3.0f);
float FPVCallbacks::cameraBaseSpeed = 5.0f;

bool FPVCallbacks::toggleTransparency = true;
bool FPVCallbacks::activeCallback = false;

glm::vec3 FPVCallbacks::offset = glm::vec3(0.0f, 0.0f, 0.0f);
float FPVCallbacks::delta = 1;


void FPVCallbacks::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (activeCallback) {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    
}

void FPVCallbacks::processInput(GLFWwindow* window, glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 cameraRight, glm::vec3 cameraPos, float deltaTime, glm::vec3 objPos)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = cameraBaseSpeed * deltaTime; // adjust accordingly

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && activeCallback == true) {
        camPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && activeCallback == true)
        camPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && activeCallback == true)
        camPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && activeCallback == true)
        camPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && activeCallback == true) {
        toggleTransparency = !toggleTransparency;
        cout << "Touche T pressee" << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        activeCallback = true;
        cout << "Touche 1 pressee" << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        activeCallback = false;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        yaw = YAW;
        pitch = PITCH;
        cameraPos= POS; 
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        offset.x = objPos.x;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        offset.y = objPos.y;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        offset.x = 0.0f;
        offset.y = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        delta -= 0.5;
        if (delta < 0) {
            delta = 0;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        delta += 0.5;
    }

}

float FPVCallbacks::getYaw()
{
    return yaw;
}

float FPVCallbacks::getPitch()
{
    return pitch;
}

bool FPVCallbacks::getTransparencyToggle()
{
    return toggleTransparency;
}

glm::vec3 FPVCallbacks::getOffset()
{
    return offset;
}

float FPVCallbacks::getDelta()
{
    return delta;
}

glm::vec3 FPVCallbacks::getCamPos() { return camPos; }