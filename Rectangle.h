#pragma once
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
/*
Class : Rectangle
This class was made to easily create 2D planes in the OpenGL Scene.
You can then apply texture to them and update their position
*/
class Rectangle {
private:

	const float epsilon = 0.001;

	float width;
	float height;

	glm::vec3 normal;
	glm::vec3 center;
	glm::vec3 up;

	glm::vec3 topRight;
	glm::vec3 bottomRight;
	glm::vec3 bottomLeft;
	glm::vec3 topLeft;

	cv::Mat texMat;

	unsigned int VBO, VAO, EBO;
	unsigned int texture1;


public:
	Rectangle();
	Rectangle(float initWidth, float initHeight);
	Rectangle(float initWidth, float initHeight, glm::vec3 initNormal, glm::vec3 initCenter, glm::vec3 initUp);

	//Setting Attributes
	void setCenter(glm::vec3 newCenter);
	void setNormal(glm::vec3 newNormal);
	void setUp(glm::vec3 newUp);
	void setWHAccordingToTexture(float scale);
	void setWH(float newWidth, float newHeight);
	void setWidth(float newWidth);
	void setHeight(float newHeight);
	void scale(float scale);

	//Getting attributes
	glm::vec3 getCenter() const;
	unsigned int getTexture() const;

	//Operating methods
	void updatePos();
	void createRectangle();
	void drawRectangle(Shader &bShader);
	void bindTexture(cv::Mat img);
	void bindTexture(string path);

private:
	void recalculateCoordinates();
	void recalculateTexture(cv::Mat img);
	
};
