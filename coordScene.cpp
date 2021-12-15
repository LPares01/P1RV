#include "coordScene.h"


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

		//Distortion coefficients
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
}

bool coordScene(Mat const& frame, Mat cameraMatrix, Mat distCoefs, vector<glm::vec3> &rvecs, vector<glm::vec3> &tvecs) {
	vector<int> markerIds;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;
	Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_6X6_50);
	vector<Vec3d> rVectors, tVectors;

	bool detected;

	flip(frame, frame, 1);
	aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIds, parameters, rejectedCandidates, cameraMatrix, distCoefs);
	//cout << "Detected: " << markerIds.size() << endl;
	//cout << "Rejected: " << rejectedCandidates.size() << endl;

	if (markerIds.size() > 0)
	{
		//cout << markerIds.size() << endl;
		detected = true;
		//aruco::drawDetectedMarkers(frame, markerCorners, markerIds);
		aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDim, cameraMatrix, distCoefs, rVectors, tVectors);

		for (int i = 0; i < markerIds.size(); i++) {
			drawFrameAxes(frame, cameraMatrix, distCoefs, rVectors[i], tVectors[i], 10.0f); //ne fonctionne pas pour une raison inconnue
		}
	}
	else {
		detected = false;
	}

	//Conversion Vec3d->vec3
	for (int i = 0; i < rVectors.size(); i++) {
		Vec3d rcurrent = rVectors[i];
		Vec3d tcurrent = tVectors[i];
		rvecs.push_back(glm::vec3(rcurrent[0], rcurrent[1], rcurrent[2]));
		tvecs.push_back(glm::vec3(tcurrent[0], tcurrent[1], tcurrent[2]));
	}
	return detected;
}

bool coordScene1(Mat const& frame, Mat cameraMatrix, Mat distCoefs, vector<marker> markers, Vec3d& rvec, Vec3d& tvec) {
	vector<int> markerIds;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;
	Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_6X6_50);
	vector<Vec3d> rVectors, tVectors;

	bool detected;

	flip(frame, frame, 1);
	aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIds, parameters, rejectedCandidates, cameraMatrix, distCoefs);
	//cout << "Detected: " << markerIds.size() << endl;
	//cout << "Rejected: " << rejectedCandidates.size() << endl;

	int nbMarkers = markerIds.size();
	if (nbMarkers > 0)
	{
		detected = true;
		aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDim, cameraMatrix, distCoefs, rVectors, tVectors);

		if (nbMarkers == 1) {
			rvec = rVectors[0];
			tvec = tVectors[0];
		}
		else {
			// Extraction des marqueurs visibles dans la frame actuelle
			vector<marker> visibleMarkers;
			for (int i = 0; i < markers.size(); i++) {
				marker currentMarker = markers[i];
				int j = 0;
				while (j < nbMarkers && currentMarker.id != markerIds[j]) {
					j++;
				}
				if (j != nbMarkers) {
					visibleMarkers.push_back(currentMarker);
				}
			}

			// Conversion vector<marker> -> vector<vector<Point3f>>
			vector<vector<Point3f>> objPoints;
			for (int i = 0; i < nbMarkers; i++) {
				marker currentMarker = visibleMarkers[i];
				Point3f topleft, topright;
				vector<Point3f> newMarker;

				topleft = Point3f(currentMarker.pos0[0], currentMarker.pos0[1], currentMarker.pos0[2]);
				topright = Point3f(currentMarker.pos1[0], currentMarker.pos1[1], currentMarker.pos1[2]);

				newMarker.push_back(topleft);
				newMarker.push_back(topright);

				objPoints.push_back(newMarker);
			}

			// Selection des coins des marqueurs Aruco
			vector<vector<Point2f>> imgPoints;
			for (int i = 0; i < nbMarkers; i++) {
				Point2f topleft, topright;
				vector<Point2f> selectedCorners;

				topleft = Point2f(markerCorners[i][0].x, markerCorners[i][0].y);
				topright = Point2f(markerCorners[i][1].x, markerCorners[i][1].y);

				selectedCorners.push_back(topleft);
				selectedCorners.push_back(topright);

				imgPoints.push_back(selectedCorners);
			}			

			solvePnP(objPoints, imgPoints, cameraMatrix, distCoefs, rvec, tvec);
		}

		for (int i = 0; i < nbMarkers; i++) {
			drawFrameAxes(frame, cameraMatrix, distCoefs, rVectors[i], tVectors[i], 1.0f);
		}
	}
	else {
		detected = false;
	}
	return detected;
}

vector<glm::vec3> filtreMoy(vector<glm::vec3> rvecs, vector<glm::vec3> tvecs) {
	glm::vec3 rvecMoy = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 tvecMoy = glm::vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < rvecs.size(); i++) {
		rvecMoy += rvecs[i];
	}
	for (int i = 0; i < tvecs.size(); i++) {
		tvecMoy += tvecs[i];
	}
	
	vector<glm::vec3> res;
	res.push_back(rvecMoy);
	res.push_back(tvecMoy);
	return res;

}