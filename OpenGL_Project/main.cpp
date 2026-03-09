#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Terrain.h"
#include "Skybox.h"

// vertex shader source ground code
const char* vertexShaderSource = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec2 aTexCoord;
	out vec2 TexCoord;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
	void main() {
		// Multiplcation of matrices to convert 3D coordinates to 2D screen coordinates
		gl_Position = projection * view * model * vec4(aPos, 1.0);
		TexCoord = aTexCoord;
	}	
)";

// fragment shader source ground code
const char* fragmentShaderSource = R"(
	#version 330 core
	out vec4 FragColor;
	in vec2 TexCoord;
	
	// the loaded grass texture
	uniform sampler2D texture1;

	void main() {
		// look up the pixel color from the image using the texture coordinates and set it as the output color
		FragColor = texture(texture1, TexCoord);
	}
)";

// vertex shader source skybox code
const char* skyboxVertexShaderSource = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;
	out vec3 TexCoords;
	uniform mat4 view;
	uniform mat4 projection;
	void main() {
		TexCoords = aPos;
		vec4 pos = projection * view * vec4(aPos, 1.0);
		gl_Position = pos.xyww; // Set w to the depth value to ensure the skybox is rendered behind all other objects
	}
)";

// fragment shader source skybox code
const char* skyboxFragmentShaderSource = R"(
	#version 330 core
	out vec4 FragColor;
	in vec3 TexCoords;
	uniform samplerCube skybox;
	void main() {
		FragColor = texture(skybox, TexCoords);
	}
)";

// function that adjusts the window if the user resizes it
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// function to compile the vertex and fragment shaders and link them into a shader program
unsigned int CompileShaders() {
	// compile the vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// compile the fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// link the vertex and fragment shaders into a shader program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// delete the vertex and fragment shaders as they are no longer needed after linking
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

unsigned int CompileSkyboxShaders() {
	// compile the vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &skyboxVertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// compile the fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &skyboxFragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// link the vertex and fragment shaders into a shader program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// delete the vertex and fragment shaders as they are no longer needed after linking
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

int main() {
	// initialize GLFW
	glfwInit();
	// set the version of OpenGL to 3.3 and use the core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Project", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	// set up the vertex data and buffers for the terrain and skybox, and load the texture for the terrain
	setupHeightmapTerrain("heightmap.png");
	setupSkyboxBuffers();
	unsigned int groundTexture = LoadTexture("grass.jpg");

	// list of file paths for the 6 faces of the cubemap texture for the skybox
	std::vector<std::string> faces = {
		"bluecloud_rt.jpg", // Right
		"bluecloud_lf.jpg", // Left
		"bluecloud_up.jpg", // Top
		"bluecloud_dn.jpg", // Bottom
		"bluecloud_ft.jpg", // Front
		"bluecloud_bk.jpg"  // Back
	};

	// load the cubemap texture for the skybox
	unsigned int cubemapTexture = loadCubemap(faces);

	// compile ground and skybox shaders and link them into shader programs
	unsigned int shaderProgram = CompileShaders();
	unsigned int skyboxShaderProgram = CompileSkyboxShaders();
	
	glEnable(GL_DEPTH_TEST);

	// render loop
	while (!glfwWindowShouldClose(window)) {
		// input - check if the user has pressed the escape key to close the window
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// render
		glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ---------------------------------
		//  DRAW GROUND
		// ---------------------------------
		// bind the texture and set the shader program
		glUseProgram(shaderProgram);

		// set the model, view, and projection matrices
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 500.0f);
		glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 100.0f, 180.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 model = glm::mat4(1.0f);

		// set the uniform variables for the model, view, and projection matrices in the shader program
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// bind the texture and draw the terrain
		glBindTexture(GL_TEXTURE_2D, groundTexture);

		glBindVertexArray(terrainVAO);

		// draw the terrain using the EBO to specify the indices of the vertices to draw
		glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);

		// ---------------------------------
		//  DRAW SKYBOX
		// ---------------------------------

		glDepthFunc(GL_LEQUAL); // change depth function so the skybox is rendered behind all other objects
		glUseProgram(skyboxShaderProgram);

		glm::mat4 skyboxView = glm::mat4(glm::mat3(view)); // remove the translation component from the view matrix for the skybox

		// set the uniform variables for the view and projection matrices in the skybox shader program
		glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(skyboxView));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		// draw the skybox using the vertex data for the cube and the cubemap texture
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS); // set depth function back to default for the rest of the scene

		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}