#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include <iostream>
using namespace std;
/*
Class : FPVCallbacks
This class was made to process user entry
It is made for a FPV and has a mouse and keyboard process function
*/
static class FPVCallbacks {
private:

	//Mouse callback attributes
	static float yaw;
	static float pitch;

	static float lastX;
	static float lastY;
	static bool firstMouse;

	static float sensitivity;

	//Keyboard callback attributes
	static glm::vec3 camPos;
	static float cameraBaseSpeed;

	//DEBUG Transparency attribute
	static bool toggleTransparency;
	static bool activeCallback;

	//DEBUG Scene offsetAttributes
	static glm::vec3 offset;
	static float delta;

public:
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static float getYaw();
	static float getPitch();
	static bool getTransparencyToggle();
	static glm::vec3 getOffset();
	static float getDelta();

	static void processInput(GLFWwindow* window, glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 cameraRight, glm::vec3 cameraPosition, float deltaTime, glm::vec3 objPos);
	static glm::vec3 getCamPos();

};

