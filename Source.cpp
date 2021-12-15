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


using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main()
{
    //Loading Open CV Image
    vector<glm::vec3> rvecs, tvecs;
    cv::VideoCapture cap(0);
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
    //Creating Shader
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    Shader ghostShader("transparentShader.vs", "transparentShader.fs");

    // tell GLFW to capture our mouse
    glfwSetCursorPosCallback(window, FPVCallbacks::mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Init basic model Position
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    // load models
    Model ourModel("backpack/backpack.obj");
    Model scene("sceneHP.obj");

    //Creating background Rectangle
    Rectangle background = Rectangle();
    background.createRectangle();
    background.setWH(SCR_WIDTH, SCR_HEIGHT);
    background.scale(0.025);

    //Temporary texture for DEBUG Purpose
    //------------------------------------------------------------------------------------------------
    unsigned int tempTex;
    glGenTextures(1, &tempTex);
    cv::Mat tempTexMat = cv::imread("checkTexture.png");
    cv::cvtColor(tempTexMat, tempTexMat, cv::COLOR_RGB2BGR);//Set the texture wrapping parameters
    glBindTexture(GL_TEXTURE_2D, tempTex);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
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
        tempTexMat.ptr());			//The actual image
    glGenerateMipmap(GL_TEXTURE_2D);


    // Uncomment to draw in Wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // render loop
    while (!glfwWindowShouldClose(window))
    {
        
        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear Z-Buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // input
        FPVCallbacks::processInput(window, mainCam.getFront(), mainCam.getUp(), deltaTime);

        //Setting Camera
        mainCam.setPosition(FPVCallbacks::getCameraPos());
        mainCam.setYP(FPVCallbacks::getYaw(), FPVCallbacks::getPitch());
        mainCam.updateCameraMatrix();

        //Linking uniforms to the different shaders
        //------------------------------------------------------------------------------------------------
        
        //For ourShader
        ourShader.use();
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &mainCam.getView()[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mainCam.getProjectionMat()));

        //For the ghostShader
        ghostShader.use();
        unsigned int modelGLoc = glGetUniformLocation(ghostShader.ID, "model");
        unsigned int viewGLoc = glGetUniformLocation(ghostShader.ID, "view");
        unsigned int projectionGLoc = glGetUniformLocation(ghostShader.ID, "projection");
        unsigned int sWidthGLoc = glGetUniformLocation(ghostShader.ID, "screenWidth");
        unsigned int sHeightGLoc = glGetUniformLocation(ghostShader.ID, "screenHeight");
        glUniformMatrix4fv(viewGLoc, 1, GL_FALSE, &mainCam.getView()[0][0]);
        glUniformMatrix4fv(projectionGLoc, 1, GL_FALSE, glm::value_ptr(mainCam.getProjectionMat()));
        glUniform1f(sWidthGLoc, SCR_WIDTH);
        glUniform1f(sHeightGLoc, SCR_HEIGHT);

        //Updating and drawing background rectangle
        //------------------------------------------------------------------------------------------------
        glm::mat4 Bmodel = glm::mat4(1.0f);
        glUniformMatrix4fv(modelGLoc, 1, GL_FALSE, glm::value_ptr(Bmodel));
        glm::vec3 backgroundPos = FPVCallbacks::getCameraPos() + glm::normalize(mainCam.getFront()) * 30.0f;
        background.setCenter(backgroundPos);
        background.setNormal(mainCam.getFront());
        background.setUp(mainCam.getUp());
        background.updatePos();
        cap.read(webcamImg);
        coordScene(webcamImg, rvecs, tvecs);
        cv::flip(webcamImg, webcamImg, 1);

        background.bindTexture(webcamImg);
        background.drawRectangle(ghostShader);


        //Drawing the backback
        //------------------------------------------------------------------------------------------------
        ourShader.use();
        glm::mat4 backpackModel = glm::mat4(1.0f);
        float angle = 0;
        backpackModel = glm::translate(backpackModel, position);
        backpackModel = glm::rotate(backpackModel, glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(backpackModel));

        ourModel.Draw(ourShader);

        //Drawing the scene
        //------------------------------------------------------------------------------------------------
        ghostShader.use();
        if (FPVCallbacks::getTransparencyToggle()) {
            glBindTexture(GL_TEXTURE_2D, background.getTexture());
        }
        else {
            glBindTexture(GL_TEXTURE_2D, tempTex);
        }
        glm::mat4 sceneModel = glm::mat4(1.0f);
        sceneModel = glm::translate(sceneModel, position);
        sceneModel = glm::rotate(sceneModel, glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
        sceneModel = glm::scale(sceneModel, glm::vec3(6.0f, 6.0f, 6.0f));
        sceneModel = glm::translate(sceneModel, glm::vec3(0.0f, -0.4f, 0.0f));
        glUniformMatrix4fv(modelGLoc, 1, GL_FALSE, glm::value_ptr(sceneModel));
        scene.Draw(ghostShader);

        //Recalculate DeltaTime of the Scene
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