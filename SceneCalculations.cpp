#include "SceneCalculations.h"

SceneCalculations::SceneCalculations()
{
}

vector<marker> SceneCalculations::getsceneMarkers() const
{
    return this->sceneMarkers;
}

void SceneCalculations::readArucoPosFile(string path) {
    this->sceneMarkers.clear();

    vector<string> tempNames;
    vector<glm::vec3> tempVertices;

    //Reading file
    fstream fichier, out;
    fichier.open(path, ios::in);
    if (fichier.is_open()) {
        string line;
        while (getline(fichier, line))
        {
            vector<string> lineD = decouperLigne(line);
            //Enregistrement des noms de fichier
            if (lineD[0] == "o") {
                tempNames.push_back(lineD[1]);
            }
            //Enregistrement des vertices
            if (lineD[0] == "v") {
                glm::vec3 tempVec;
                tempVec.x = stof(lineD[1]);
                tempVec.y = stof(lineD[2]);
                tempVec.z = stof(lineD[3]);
                tempVertices.push_back(tempVec);
            }

        }
    }

    fichier.close();

    //struct verticeObject {
    //    int id = 0;
    //    glm::vec3 normalVert = glm::vec3(0.0f, 0.0f, 0.0f);
    //    glm::vec3 centerVert = glm::vec3(0.0f, 0.0f, 0.0f);
    //    glm::vec3 rightVert = glm::vec3(0.0f, 0.0f, 0.0f);
    //};

    struct verticeObject {
        int id = 0;
        glm::vec3 topleft = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 topright = glm::vec3(0.0f, 0.0f, 0.0f);
    };

    vector<verticeObject> verticeObjectList;

    //Linking vertices between them according to their ID
    for (int i = 0; i < tempNames.size(); i++) {
        vector<string> info = decouperNom(tempNames[i]);
        bool registered = false;
        int j = 0;
        while (!registered && j < verticeObjectList.size()) {
            if (verticeObjectList[j].id == stoi(info[0])) {
                registered = true;
            }
            else {
                j++;
            }
        }
        if (!registered) {
            verticeObject newObject;
            newObject.id = stoi(info[0]);
            verticeObjectList.push_back(newObject);
            j = verticeObjectList.size() - 1;
        }

        //Finding what type of element this vertice represent
        //if (info[1] == "center") {
        //    verticeObjectList[j].centerVert = tempVertices[i];
        //}
        //else if (info[1] == "normal") {
        //    verticeObjectList[j].normalVert = tempVertices[i];
        //}
        //else if (info[1] == "right") {
        //    verticeObjectList[j].rightVert = tempVertices[i];
        //}
        if (info[1] == "center") {
            verticeObjectList[j].topleft = tempVertices[i];
        } else if (info[1] == "right") {
            verticeObjectList[j].topright = tempVertices[i];
        }
    }

    //For DEBUG
    cout << "------Lecture des positions virtuelles des marqueurs Aruco" << endl;
    for (int i = 0; i < verticeObjectList.size(); i++) {
        cout << "Propriétés du marqueur:" << endl;
        cout << "id: " << verticeObjectList[i].id << endl;
        //cout << "Center Vert: " << verticeObjectList[i].centerVert.x << " " << verticeObjectList[i].centerVert.y << " " << verticeObjectList[i].centerVert.z << endl;
        //cout << "Normal Vert: " << verticeObjectList[i].normalVert.x << " " << verticeObjectList[i].normalVert.y << " " << verticeObjectList[i].normalVert.z << " " << endl;
        //cout << "Right Vert: " << verticeObjectList[i].rightVert.x << " " << verticeObjectList[i].rightVert.y << " " << verticeObjectList[i].rightVert.z << " " << endl;
        cout << "Topleft Vert: " << verticeObjectList[i].topleft.x << " " << verticeObjectList[i].topleft.y << " " << verticeObjectList[i].topleft.z << endl;
        cout << "Topright Vert: " << verticeObjectList[i].topright.x << " " << verticeObjectList[i].topright.y << " " << verticeObjectList[i].topright.z << endl;
        cout << "------------------------------------------------------------------------" << endl << endl;

    }
    //Converting verticeObject To markers
    for (int i = 0; i < verticeObjectList.size(); i++) {
        marker newMarker;
        newMarker.id = verticeObjectList[i].id;
        newMarker.pos0 = verticeObjectList[i].topleft;
        newMarker.pos1 = verticeObjectList[i].topright;
        /*newMarker.position = verticeObjectList[i].centerVert;
        newMarker.normal = glm::normalize(verticeObjectList[i].normalVert - verticeObjectList[i].centerVert);
        newMarker.right = glm::normalize(verticeObjectList[i].rightVert - verticeObjectList[i].centerVert);*/

        sceneMarkers.push_back(newMarker);
    }
}

vector<string> SceneCalculations::decouperLigne(string line) {
    vector<string> res;

    string temp;
    for (int i = 0; i < line.size(); i++)
    {
        if (line[i] != ' ') {
            temp.push_back(line[i]);
        }
        else {
            res.push_back(temp);
            temp = "";
        }
    }
    res.push_back(temp);
    return res;
}

vector<string> SceneCalculations::decouperNom(string name) {
    vector<string> res;
    string temp = "";
    for (int i = 0; i < name.size(); i++) {
        if (name[i] == '.' || name[i] == '_') {
            res.push_back(temp);
            temp = "";
        }
        else {
            temp.push_back(name[i]);
        }
    }
    return res;
}