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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
cv::VideoCapture cap(0);

const unsigned int SCR_WIDTH = cap.get(CAP_PROP_FRAME_WIDTH);
const unsigned int SCR_HEIGHT = cap.get(CAP_PROP_FRAME_HEIGHT);


float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main()
{
    //Loading Open CV Image
    
    cv::Mat webcamImg;
    cap.read(webcamImg);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "P1RV", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);



    //Creating Camera
    Camera mainCam = Camera();

    Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    Mat distCoefs;
    float f_x, f_y, c_x, c_y;
    loadCameraCalib("calib_data", cameraMatrix, distCoefs);
    // Extraction des coefficients de distorsion
    f_x = cameraMatrix.at<double>(0, 0); //cout << f_x << endl;
    c_x = cameraMatrix.at<double>(0, 2); //cout << c_x << endl;
    f_y = cameraMatrix.at<double>(1, 1); //cout << f_y << endl;
    c_y = cameraMatrix.at<double>(1, 2); //cout << c_y << endl;

    float FOVY = 2 * atan(SCR_HEIGHT / (2 * f_y));

    mainCam.setPerspectiveProjection(FOVY, SCR_WIDTH, SCR_HEIGHT, NEAR, FAR);
   

    //Creating Shader
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    Shader ghostShader("transparentShader.vs", "transparentShader.fs");

    // Tell GLFW to capture our mouse
    glfwSetCursorPosCallback(window, FPVCallbacks::mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Init basic model Position
    glm::vec3 posToCam = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotToCam = glm::vec3(0.0f, 0.0f, 0.0f);

    //float angleX, angleY, angleZ;
    glm::vec3 rotAxis = glm::vec3(0.0f, 0.0f, 0.0f);

    // Tell stb_image.h to flip loaded texture's on the y-axis (before loading model)
    stbi_set_flip_vertically_on_load(true);
    // Load models
    Model scene("suzanne.obj");


    // Creating background Rectangle
    Rectangle background = Rectangle();
    background.createRectangle();
    background.setWH(SCR_WIDTH, SCR_HEIGHT);
    background.scale(0.080f);

    // Temporary texture for DEBUG Purpose
    //------------------------------------------------------------------------------------------------
    unsigned int tempTex;
    glGenTextures(1, &tempTex);
    cv::Mat tempTexMat = cv::imread("checkTexture.png");
    cv::cvtColor(tempTexMat, tempTexMat, cv::COLOR_RGB2BGR); //Set the texture wrapping parameters
    glBindTexture(GL_TEXTURE_2D, tempTex);
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
        0,					//Pyramid level (for mip-mapping)
        GL_RGB,				// Internal colour format to convert to
        tempTexMat.cols,			// Image width
        tempTexMat.rows,			// Image height
        0,					//Border with pixel (either 0 or 1)
        GL_RGB,				//Input image format
        GL_UNSIGNED_BYTE,	//Image data type
        tempTexMat.ptr());			//The current frame
    glGenerateMipmap(GL_TEXTURE_2D);


    // Uncomment to draw in Wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // Render loop
    while (!glfwWindowShouldClose(window))
    {

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Z-Buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Input
        FPVCallbacks::processInput(window, mainCam.getFront(), mainCam.getUp(), mainCam.getRight(), mainCam.getPos(), deltaTime, posToCam);

        // Setting Camera
        mainCam.setPosition(FPVCallbacks::getCamPos());
        mainCam.setYP(FPVCallbacks::getYaw(), FPVCallbacks::getPitch());
        mainCam.updateCameraMatrix();

        // Linking uniforms to the different shaders
        //------------------------------------------------------------------------------------------------

        // For ourShader
        ourShader.use();
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &mainCam.getView()[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mainCam.getProjectionMat()));
        //glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        // For the ghostShader
        ghostShader.use();
        unsigned int modelGLoc = glGetUniformLocation(ghostShader.ID, "model");
        unsigned int viewGLoc = glGetUniformLocation(ghostShader.ID, "view");
        unsigned int projectionGLoc = glGetUniformLocation(ghostShader.ID, "projection");
        unsigned int sWidthGLoc = glGetUniformLocation(ghostShader.ID, "screenWidth");
        unsigned int sHeightGLoc = glGetUniformLocation(ghostShader.ID, "screenHeight");
        glUniformMatrix4fv(viewGLoc, 1, GL_FALSE, &mainCam.getView()[0][0]);
        glUniformMatrix4fv(projectionGLoc, 1, GL_FALSE, glm::value_ptr(mainCam.getProjectionMat()));
        //glUniformMatrix4fv(projectionGLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniform1f(sWidthGLoc, SCR_WIDTH);
        glUniform1f(sHeightGLoc, SCR_HEIGHT);

        //Updating (and drawing) the background rectangle
        //------------------------------------------------------------------------------------------------

        glm::mat4 Bmodel = glm::mat4(1.0f);
        glUniformMatrix4fv(modelGLoc, 1, GL_FALSE, glm::value_ptr(Bmodel));
        glm::vec3 backgroundPos = FPVCallbacks::getCamPos() + glm::normalize(mainCam.getFront()) * 40.0f;
        background.setCenter(backgroundPos);
        background.setNormal(mainCam.getFront());
        background.setUp(mainCam.getUp());
        background.updatePos();
        cap.read(webcamImg);
        flip(webcamImg, webcamImg, 1);
        background.bindTexture(webcamImg);
        background.drawRectangle(ghostShader);


        //Drawing the scene (and the background rectangle)
        //------------------------------------------------------------------------------------------------  
        ghostShader.use();
        if (FPVCallbacks::getTransparencyToggle()) {
            glBindTexture(GL_TEXTURE_2D, background.getTexture());
        }
        else {
            glBindTexture(GL_TEXTURE_2D, tempTex);
        }
        
        glm::mat4 sceneModel = glm::mat4(1.0f);

        bool detected = false;
        // Pour coordScene
        //vector<glm::vec3> rvecs, tvecs;
        //detected = coordScene(webcamImg, cameraMatrix, distCoefs, rvecs, tvecs);
        ////cout << detected << endl;

        // Pour coordScene1
        Vec3d rvec, tvec;
        vector<marker> markers;
        SceneCalculations sceneCalc;
        markers = sceneCalc.getsceneMarkers();
        detected = coordScene1(webcamImg, cameraMatrix, distCoefs, markers, rvec, tvec);
        cout << detected << endl;


        // CHANGED OF PLACE !!
/*      flip(webcamImg, webcamImg, 1);
        background.bindTexture(webcamImg);
        background.drawRectangle(ghostShader);  */


        // Pour coordScene
        //if (detected) {
        //    posToCam.x = tvecs[0][0];
        //    posToCam.y = tvecs[0][1];
        //    posToCam.z = tvecs[0][2];

        //    rotToCam.x = rvecs[0][0];
        //    rotToCam.y = rvecs[0][1];
        //    rotToCam.z = rvecs[0][2];
        //}


        // Pour coordScene1
        if (detected) {
            // Une solution au problème de rotation ?
            //Mat R, tvecMat;
            //Rodrigues(rvec, R);
            //R = R.t();
            //tvecMat = -R * Mat(tvec);
            //tvec = tvecMat.reshape(3).at<cv::Vec3d>();
            //Rodrigues(R, rvec);

            posToCam.x = tvec[0];
            posToCam.y = tvec[1];
            posToCam.z = tvec[2];

            rotToCam.x = rvec[0];
            rotToCam.y = rvec[1];
            rotToCam.z = rvec[2];
        }

        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        position += posToCam.x * (glm::normalize(mainCam.getRight()) * FPVCallbacks::getDelta());
        position += posToCam.y * (glm::normalize(-mainCam.getUp()) * FPVCallbacks::getDelta());
        position += posToCam.z * (glm::normalize(mainCam.getFront()) * FPVCallbacks::getDelta());
        position += mainCam.getPos();

        position -= FPVCallbacks::getOffset().x * glm::normalize(-mainCam.getUp());
        position -= FPVCallbacks::getOffset().y * glm::normalize(mainCam.getRight());

        rotAxis = glm::vec3(0.0f, 0.0f, 0.0f);
        rotAxis += rotToCam.x * glm::normalize(mainCam.getRight());
        rotAxis += rotToCam.y * glm::normalize(-mainCam.getUp());
        rotAxis += rotToCam.z * glm::normalize(mainCam.getFront());

        sceneModel = glm::translate(sceneModel, position);
        sceneModel = glm::rotate(sceneModel, glm::length(rotToCam), glm::normalize(rotAxis));
        sceneModel = glm::scale(sceneModel, glm::vec3(0.02f, 0.02f, 0.02f));
        glUniformMatrix4fv(modelGLoc, 1, GL_FALSE, glm::value_ptr(sceneModel));
        scene.Draw(ghostShader);

        // Recalculate DeltaTime of the Scene
        //------------------------------------------------------------------------------------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //------------------------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    //----------------------------------------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// -------------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}