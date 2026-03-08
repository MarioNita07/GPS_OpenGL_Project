#pragma once

// variables for terrain rendering
extern float groundVertices[20];
extern unsigned int groundIndices[6];

// VAO, VBO, and EBO for the terrain
extern unsigned int groundVAO, groundVBO, groundEBO;

// function to set up the vertex data and buffers for the terrain
void setupGroundBuffers();

// function to load a texture from a file and return the texture ID
unsigned int LoadTexture(const char* filename);