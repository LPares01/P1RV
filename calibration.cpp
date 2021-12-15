#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>

#include <sstream>
#include <iostream>
#include <fstream>


using namespace std;
using namespace cv;

const float calibSquareDim = 0.0240f; // meters
const float arucoSquareDim = 0.0195f; // meters
const Size chessboardDim = Size(6, 9);

Ptr<aruco::Dictionary> createArucoMarkers() {
	Mat outputMarker;
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_6X6_50);
	for (int i = 0; i < 50; i++) {
		aruco::drawMarker(markerDictionary, i, 500, outputMarker, 1);
		ostringstream convert;
		string imgName = "6x6Marker_";
		convert << imgName << i << ".jpg";
		imwrite(convert.str(), outputMarker);
	}
	return markerDictionary;
}

void createKnownBoardPos(Size boardSize, float squareEdgeLength, vector<Point3f>& corners) {
	for (int i = 0; i < boardSize.height; i++) {
		for (int j = 0; j < boardSize.width; j++) {
			corners.push_back(Point3f(j * squareEdgeLength, i * squareEdgeLength, 0.0f));
		}
	}
}

void getChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& allFoundCorners, bool showResults) {
	for (vector<Mat>::iterator iter = images.begin(); iter != images.end(); iter++) {
		vector<Point2f> pointBuf;
		bool found = findChessboardCorners(*iter, chessboardDim, pointBuf, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
		if (found) {
			allFoundCorners.push_back(pointBuf);
		}
		if (showResults) {
			drawChessboardCorners(*iter, chessboardDim, pointBuf, found);
			imshow("Looking for corners", *iter);
			waitKey(0);
		}
	}
}

int startWebcamMonitoring(const Mat& cameraMatrix, const Mat& distCoefs, float arucoSquareDim) {
	Mat frame;

	vector<int> markerIds;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;
	aruco::DetectorParameters parameters;

	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_6X6_1000);

	VideoCapture vid(0);

	if (!vid.isOpened()) {
		return -1;
	}
	namedWindow("Webcam", WINDOW_AUTOSIZE);

	vector<Vec3d> rVectors, tVectors;

	while (true) {
		if (!vid.read(frame)) {
			break;
		}

		aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIds);
		aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDim, cameraMatrix, distCoefs, rVectors, tVectors);

		for (int i = 0; i < markerIds.size(); i++) {
			aruco::drawAxis(frame, cameraMatrix, distCoefs, rVectors[i], tVectors[i], 0.1f);
		}
		imshow("Webcam", frame);
		if (waitKey(30) >= 0) { break; }
	}
	return 1;
}

void cameraCalib(vector<Mat> calibImages, Size boardSize, float squareEdgeLength, Mat& cameraMatrix, Mat& distCoefs, vector<Mat> rVectors, vector<Mat> tVectors) {
	vector<vector<Point2f>> cbSpacePoints;
	getChessboardCorners(calibImages, cbSpacePoints, false);

	vector<vector<Point3f>> worldSpacePoints(1);

	createKnownBoardPos(boardSize, squareEdgeLength, worldSpacePoints[0]);
	worldSpacePoints.resize(cbSpacePoints.size(), worldSpacePoints[0]);

	distCoefs = Mat::zeros(8, 1, CV_64F);

	calibrateCamera(worldSpacePoints, cbSpacePoints, boardSize, cameraMatrix, distCoefs, rVectors, tVectors);
}

bool saveCameraCalib(string name, Mat cameraMatrix, Mat distCoefs) {
	ofstream outstream(name);
	if (outstream) {
		//string prefixe = "";
		uint16_t rows = cameraMatrix.rows;
		uint16_t cols = cameraMatrix.cols;

		outstream << rows << endl;
		outstream << cols << endl;

		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < cols; c++) {
				/*if (r == 0 && c == 0) {
					prefixe = "fx = ";
				}
				else if (r == 0 && c == 1) {
					prefixe = "cx = ";
				}
				else if (r == 1 && c == 1) {
					prefixe = "fy = ";
				}
				else if (r == 1 && c == 2) {
					prefixe = "cy = ";
				}
				else {
					prefixe = "";
				}*/
				double value = cameraMatrix.at<double>(r, c);
				outstream /* << prefixe*/ << value << endl;
			}
		}

		rows = distCoefs.rows;
		cols = distCoefs.cols;
		//outstream << "Distortion coefficients :" << endl;
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < cols; c++) {
				double value = distCoefs.at<double>(r, c);
				outstream << value << endl;
			}
		}

		outstream.close();
		return true;
	}
	return false;
}//ajouter rVectors et tVectors

bool loadCameraCalib(string name, Mat& cameraMatrix, Mat& distCoefs) {
	ifstream instream(name);
	if (instream) {
		uint16_t rows, cols;

		instream >> rows;
		instream >> cols;

		cameraMatrix = Mat(Size(cols, rows), CV_64F);

		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < cols; c++) {
				double read = 0.0f;
				instream >> read;
				cameraMatrix.at<double>(r, c) = read;
				//cout << cameraMatrix.at<double>(r, c) << endl;
			}
		}

		//Distance coefficients
		instream >> rows;
		instream >> cols;

		distCoefs = Mat::zeros(rows, cols, CV_64F);

		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < cols; c++) {
				double read = 0.0f;
				instream >> read;
				distCoefs.at<double>(r, c) = read;
				//cout << distCoefs.at<double>(r, c) << endl;
			}
		}

		instream.close();
		return true;
	}

	return false;
}//idem

void cameraCalibProcess(Mat& cameraMatrix, Mat& distCoefs) {
	Mat frame;
	Mat drawToFrame;

	vector<Mat> rVectors, tVectors;

	vector<Mat> savedImages;

	vector<vector<Point2f>> markerCorners, rejectedCandidates;

	int nbImg = 0;

	VideoCapture vid(0);

	if (!vid.isOpened()) {
		return;
	}

	int framesPerSecond = 20;

	namedWindow("Webcam", WINDOW_AUTOSIZE);

	while (true) {
		if (!vid.read(frame)) {
			break;
		}

		vector<Vec2f> foundPoints;
		bool found = false;

		found = findChessboardCorners(frame, chessboardDim, foundPoints, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
		frame.copyTo(drawToFrame);
		drawChessboardCorners(drawToFrame, chessboardDim, foundPoints, found);
		if (found) {
			imshow("Webcam", drawToFrame);
		}
		else {
			imshow("Webcam", frame);
		}
		char character = waitKey(1000 / framesPerSecond);

		switch (character) {
		case ' ':
			//Saving image
			if (found) {
				Mat temp;
				frame.copyTo(temp);
				if (nbImg < 10)
					imwrite("./CalibImgs/calib0" + to_string(nbImg) + ".jpg", frame);
				else
					imwrite("./CalibImgs/calib" + to_string(nbImg) + ".jpg", frame);
				savedImages.push_back(temp);
				nbImg++;
				cout << "Cheers !" << endl;
				cout << nbImg << " photo(s) taken. Press Enter to calibrate. " << endl;
			}
			break;
		case 13:
			//Start calibration
			if (savedImages.size() > 15) {
				cout << "Calibrating..." << endl;
				cameraCalib(savedImages, chessboardDim, calibSquareDim, cameraMatrix, distCoefs, rVectors, tVectors);
				saveCameraCalib("calib_data", cameraMatrix, distCoefs);
				cout << "Calibration done! You can keep taking photos or exit with ESC." << endl;
			}
			break;
		case 27:
			//Exit
			return;
			break;
		}
	}
}

int main(int argv, char** argc) {

	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);

	Mat distCoefs;

	cameraCalibProcess(cameraMatrix, distCoefs);
	//loadCameraCalib("calib_data", cameraMatrix, distCoefs);
	//startWebcamMonitoring(cameraMatrix, distCoefs, arucoSquareDim);

	return 0;
}
