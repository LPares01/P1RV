#include "Camera.h"

Camera::Camera() {

	//Initialising floating values
	this->yaw = YAW;
	this->pitch = PITCH;
	this->zoom = ZOOM;

	//Initialising vector values
	this->worldUp = UP;
	this->position = POS;
	this->front = FRONT;
	updateCameraVectors();


	//Initialising matrix values;
	projection = glm::mat4(1.0f);
	projection = glm::perspective(FOV, WIDTH / HEIGHT, NEAR, FAR);
	view = glm::lookAt(this->position, this->position + front, up);
}

void Camera::setPosition(glm::vec3 newPosition) { this->position = newPosition; }
void Camera::setPosition(float newX, float newY, float newZ) {this->position = glm::vec3(newX, newY, newZ);}
void Camera::setYaw(float newYaw) { this->yaw = newYaw; }
void Camera::setPitch(float newPitch) { this->pitch = newPitch; }
void Camera::setYP(float newYaw, float newPitch)
{
	this->yaw = newYaw;
	this->pitch = newPitch;
}

glm::mat4 Camera::getProjectionMat() const { return this->projection; }
glm::mat4 Camera::getView() const
{
	return this->view;
}
glm::vec3 Camera::getFront() const
{
	return this->front;
}
glm::vec3 Camera::getUp() const
{
	return this->up;
}
glm::vec3 Camera::getPos() const
{
	return this->position;
}
glm::vec3 Camera::getRight() const
{
	return this->right;
}
float Camera::getYaw() const
{
	return this->yaw;
}
float Camera::getPitch() const
{
	return this->pitch;
}
void Camera::setPerspectiveProjection(float newFov, float newWidth, float newHeight, float newNear, float newFar) {
	this->projection = glm::perspective(newFov, newWidth / newHeight, newNear, newFar);
}
void Camera::updateCameraMatrix() {
	updateCameraVectors();
	view = lookAt(this->position, this->position + this->front, this->up);
}
void Camera::updateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 tempFront;
	tempFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	tempFront.y = sin(glm::radians(pitch));
	tempFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(tempFront);
	// also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}