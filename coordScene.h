#pragma once

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <iostream>
#include <numeric>
#include <vector>
#include <math.h>
#include <fstream>

#include "SceneCalculations.h"

//using namespace std;
using namespace cv;
using namespace std;
#define arucoSquareDim 0.029 // meters

bool loadCameraCalib(string name, Mat& cameraMatrix, Mat& distCoefs);

bool coordScene(Mat const& frame, Mat cameraMatrix, Mat distCoefs, vector<glm::vec3>& rvecs, vector<glm::vec3>& tvecs);
bool coordScene1(Mat const& frame, Mat cameraMatrix, Mat distCoefs, vector<marker> markers, Vec3d& rvec, Vec3d& tvec);
vector<glm::vec3> filtreMoy(vector<glm::vec3> rvecs, vector<glm::vec3> tvecs);