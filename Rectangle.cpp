#include "Rectangle.h"

Rectangle::Rectangle()
{
	this->width = 10.0f;
	this->height = 5.0f;

	this->normal = glm::vec3(0.0f, 1.0f, 0.0f);
	this->center = glm::vec3(0.0f, 0.0f, 0.0f);
	this->up = glm::vec3(1.0f, 0.0f, 0.0f);

	//Generating VAO,VBO,EBO and texture
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glGenTextures(1, &texture1);

	//Calculating vertices position
	recalculateCoordinates();

}

Rectangle::Rectangle(float initWidth, float initHeight)
{
	this->width = initWidth;
	this->height = initHeight;

	this->normal = glm::vec3(0.0f, 1.0f, 0.0f);
	this->center = glm::vec3(0.0f, 0.0f, 0.0f);
	this->up = glm::vec3(1.0f, 0.0f, 0.0f);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glGenTextures(1, &texture1);

	recalculateCoordinates();
}

Rectangle::Rectangle(float initWidth, float initHeight, glm::vec3 initNormal, glm::vec3 initCenter, glm::vec3 initUp)
{
	this->width = initWidth;
	this->height = initHeight;

	this->normal = initNormal;
	this->center = initCenter;
	this->up = initUp;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glGenTextures(1, &texture1);

	recalculateCoordinates();
}

void Rectangle::setCenter(glm::vec3 newCenter) { this->center = newCenter; }

void Rectangle::setNormal(glm::vec3 newNormal) { this->normal = glm::normalize(newNormal); }

void Rectangle::setUp(glm::vec3 newUp)
{
	this->up = glm::normalize(newUp);
}

void Rectangle::setWHAccordingToTexture(float scale)
{
	if (this->texMat.empty()) {
		cout << "Can't resize rectangle to texture: No texture is bond yet ! " << endl;
	}
	else {
		this->width = this->texMat.cols * scale / 20;
		this->height = this->texMat.rows * scale / 20;
	}
}

void Rectangle::setWH(float newWidth, float newHeight) { 
	this->width = newWidth;
	this->height = newHeight;
}

void Rectangle::setWidth(float newWidth) { this->width = newWidth; }

void Rectangle::setHeight(float newHeight) { this->height = newHeight; }

void Rectangle::scale(float scale)
{
	this->width *= scale;
	this->height *= scale;
}

glm::vec3 Rectangle::getCenter() const
{
	return this->center;
}

unsigned int Rectangle::getTexture() const
{
	return this->texture1;
}

void Rectangle::createRectangle()
{
	recalculateCoordinates();

	float vertices[] = {
		//Positions										//colors				//texture coords
		 topRight.x,  topRight.y, topRight.z,			1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		 bottomRight.x, bottomRight.y, bottomRight.z,	1.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		bottomLeft.x, bottomLeft.y, bottomLeft.z,		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		topLeft.x,  topLeft.y, topLeft.z,				1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};


	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

}

void Rectangle::updatePos()
{
	recalculateCoordinates();

	float vertices[] = {
		//Positions										//colors				//texture coords
		topRight.x,  topRight.y, topRight.z,			1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		bottomRight.x, bottomRight.y, bottomRight.z,	1.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		bottomLeft.x, bottomLeft.y, bottomLeft.z,		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		topLeft.x,  topLeft.y, topLeft.z,				1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
	};


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]); //Permet de changer les valeurs dynamiquement
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Rectangle::recalculateCoordinates()
{
	glm::vec3 leftVec = glm::normalize(glm::cross(normal, up));

	this->topRight = glm::vec3(center + (height / 2) * up + (-width / 2) * leftVec);
	this->bottomRight = center + (-height / 2) * up + (-width / 2) * leftVec;
	this->bottomLeft = center + (-height / 2) * up + (width / 2) * leftVec;
	this->topLeft = center + (+height / 2) * up + (width / 2) * leftVec;
}

void Rectangle::drawRectangle(Shader &bShader){
	
	//Data relative to the shader
	bShader.use();

	//Apparently unnecessary
	//glActiveTexture(GL_TEXTURE0);
	//glUniform1i(glGetUniformLocation(bShader.ID, "texture1"), 0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	
	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
}

void Rectangle::bindTexture(string path)
{
	cv::Mat img = cv::imread(path);
	bindTexture(img);
}

void Rectangle::bindTexture(cv::Mat img)
{
	this->texMat = img;
	recalculateTexture(img.clone());
}

void Rectangle::recalculateTexture(cv::Mat img)
{
	this->texMat = img;
	
	cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
	cv::flip(img, img, -1); //Flipping along y axis
	


	//Set the texture wrapping parameters
	glBindTexture(GL_TEXTURE_2D, this->texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,
		0,					//Pyramid level (for mip-mapping)
		GL_RGB,				// Internal colour format to convert to
		img.cols,			// Image width
		img.rows,			// Image height
		0,					//Border with pixel (either 0 or 1)
		GL_RGB,				//Input image format
		GL_UNSIGNED_BYTE,	//Image data type
		img.ptr());			//The actual image
	glGenerateMipmap(GL_TEXTURE_2D);

}
