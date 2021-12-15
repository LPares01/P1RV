//OpenGL Includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//OpenCV Includes
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <string>

#include "stb_image.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Rectangle.h"
#include "FPVCallbacks.h"
#include "coordScene.h"
#include "SceneCalculations.h"


using namespace std;

int main() {
	SceneCalculations calculations;
	calculations.readArucoPosFile("Markers.obj");
}