#include "Terrain.h"
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"


float groundVertices[20] = {
	50.0f, 0.0f, 50.0f, 10.0f, 10.0f, // Top Right
	50.0f, 0.0f, -50.0f, 10.0f, 0.0f, // Bottom Right
	-50.0f, 0.0f, -50.0f, 0.0f, 0.0f, // Bottom Left
	-50.0f, 0.0f, 50.0f, 0.0f, 10.0f // Top Left
};

unsigned int groundIndices[6] = {
	0, 1, 3, // First Triangle
	1, 2, 3 // Second Triangle
};

// VAO, VBO, and EBO for the terrain
unsigned int groundVAO, groundVBO, groundEBO;

// function to set up the vertex data and buffers for the terrain
void setupGroundBuffers() {
	// generate and bind the VAO, VBO, and EBO for the terrain
	glGenVertexArrays(1, &groundVAO);
	glGenBuffers(1, &groundVBO);
	glGenBuffers(1, &groundEBO);
	glBindVertexArray(groundVAO);
	glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);

	// set the vertex attribute pointers for the terrain
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// set the vertex attribute pointers for the texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// unbind the VAO to prevent accidental modification
	glBindVertexArray(0);
}

unsigned int LoadTexture(const char* filename) {
	unsigned int textureID;
	// generate a texture ID and bind it to the 2D texture target
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// variables to hold image data
	int width, height, nrChannels;

	// flip the image vertically on load
	stbi_set_flip_vertically_on_load(true);

	// load the image data
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

	// check if the image was loaded successfully
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture: " << filename << std::endl;
	}

	// free the image data and return the texture ID
	stbi_image_free(data);

	return textureID;
}