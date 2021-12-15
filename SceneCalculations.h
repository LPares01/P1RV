#pragma once
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

//struct marker {
//	string id;
//	glm::vec3 position;
//	glm::vec3 normal; 
//	glm::vec3 right;
//};

// Pour coordScene1 :

struct marker {
	int id; // plus pratique que string
	glm::vec3 pos0; // topleft corner
	glm::vec3 pos1; // topright corner
};


class SceneCalculations {
private:
	vector<marker> sceneMarkers;
public:
	SceneCalculations();

	//Getting functions
	vector<marker> getsceneMarkers() const;

	//void calculateOrigin(vector<marker> arucoDetection);
	void readArucoPosFile(string path);
	
private:
	vector<string> decouperNom(string name);
	vector<string> decouperLigne(string line);
};