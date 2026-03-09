#pragma once
#include <vector>

// variables for ground rendering
extern float groundVertices[20];
extern unsigned int groundIndices[6];

// VAO, VBO, and EBO for the ground
extern unsigned int groundVAO, groundVBO, groundEBO;

// variables for terrain rendering
extern unsigned int terrainVAO, terrainVBO, terrainEBO;
extern unsigned int terrainIndexCount; // variable to hold the number of indices for the terrain mesh

// function to set up the vertex data and buffers for the terrain
void setupGroundBuffers();

void setupHeightmapTerrain(const char* filepath);  //function to set up the vertex data and buffers for the terrain mesh generated from a heightmap image

// function to load a texture from a file and return the texture ID
unsigned int LoadTexture(const char* filename);