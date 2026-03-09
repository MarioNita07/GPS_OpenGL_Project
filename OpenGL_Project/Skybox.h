#pragma once

#include <vector>
#include <string>

// variables for skybox rendering
extern float skyboxVertices[108];

// VAO and VBO for the skybox
extern unsigned int skyboxVAO, skyboxVBO;

// function to set up the vertex data and buffers for the skybox
void setupSkyboxBuffers();

// function to load a cubemap texture from 6 individual texture faces and return the texture ID
unsigned int loadCubemap(std::vector<std::string> faces);