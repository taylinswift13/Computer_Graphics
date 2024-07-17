
#include <GL/glew.h>

// STB_IMAGE for loading images of many filetypes
#include <stb_image.h>

#include <cstdlib>

#include <labhelper.h>

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// Various globals
///////////////////////////////////////////////////////////////////////////////

// The window we'll be rendering to
SDL_Window* g_window = nullptr;

int mag = 1;
int mini = 5;
float anisotropy = 16.0f;
float camera_pan = 0.f;
bool showUI = false;


///////////////////////////////////////////////////////////////////////////////
// Shader programs
///////////////////////////////////////////////////////////////////////////////

// The shaderProgram holds the vertexShader and fragmentShader
GLuint shaderProgram;


///////////////////////////////////////////////////////////////////////////////
// Scene objects
///////////////////////////////////////////////////////////////////////////////

// The vertexArrayObject here will hold the pointers to
// the vertex data (in positionBuffer) and color data per vertex (in colorBuffer)
GLuint positionBuffer, colorBuffer, indexBuffer, vertexArrayObject, texCoordBuffer, texture_road, positionBuffer_2, texCoordBuffer_2, vertexArrayObject_2, texture_exp;




///////////////////////////////////////////////////////////////////////////////
/// This function is called once at the start of the program and never again
///////////////////////////////////////////////////////////////////////////////
void initialize()
{
	ENSURE_INITIALIZE_ONLY_ONCE();
	//	ROAD INITIALIZE
	// 
	///////////////////////////////////////////////////////////////////////////
	// Create the vertex array object
	///////////////////////////////////////////////////////////////////////////
	// Create a handle for the vertex array object
	glGenVertexArrays(1, &vertexArrayObject);
	// Set it as current, i.e., related calls will affect this object
	glBindVertexArray(vertexArrayObject);

	///////////////////////////////////////////////////////////////////////////
	// Create the positions buffer object
	///////////////////////////////////////////////////////////////////////////
	const float positions[] = {
		// X      Y       Z
		-10.0f, 0.0f, -10.0f,  // v0
		-10.0f, 0.0f, -330.0f, // v1
		10.0f,  0.0f, -330.0f, // v2
		10.0f,  0.0f, -10.0f   // v3
	};
	// Create a handle for the vertex position buffer
	glGenBuffers(1, &positionBuffer);
	// Set the newly created buffer as the current one
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	// Send the vetex position data to the current buffer
	glBufferData(GL_ARRAY_BUFFER, labhelper::array_length(positions) * sizeof(float), positions,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false /*normalized*/, 0 /*stride*/, 0 /*offset*/);
	// Enable the attribute
	glEnableVertexAttribArray(0);
	///////////////////////////////////////////////////////////////////////////
	// Task 1 : Create the texture coordinates.
	//			Create the texture coordinates' buffer object.
	//			Set up the attrib pointer.
	//			Enable the vertex attrib array.
	///////////////////////////////////////////////////////////////////////////
	float texcoords[] = {
	0.0f, 0.0f,    // (u,v) for v0
	0.0f, 15.0f,   // (u,v) for v1
	1.0f, 15.0f,   // (u,v) for v2
	1.0f, 0.0f     // (u,v) for v3
	};
	glGenBuffers(1, &texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, labhelper::array_length(texcoords) * sizeof(float), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, false/*normalized*/, 0/*stride*/, 0/*offset*/);
	glEnableVertexAttribArray(1);
	///////////////////////////////////////////////////////////////////////////
	// Create the element array buffer object
	///////////////////////////////////////////////////////////////////////////
	const int indices[] = {
		0, 2, 1, // Triangle 1
		0, 3, 2  // Triangle 2
	};
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, labhelper::array_length(indices) * sizeof(float), indices,
		GL_STATIC_DRAW);


	// The loadShaderProgram and linkShaderProgam functions are defined in glutil.cpp and
	// do exactly what we did in lab1 but are hidden for convenience
	shaderProgram = labhelper::loadShaderProgram("../lab2-textures/simple.vert",
		"../lab2-textures/simple.frag");

	//**********************************************

	//************************************
	//			Load Texture
	//************************************
	// Task 2
	int w, h, comp;
	unsigned char* image = stbi_load("../scenes/nlsFinland/L3123F.png", &w, &h, &comp, STBI_rgb_alpha);
	glGenTextures(1, &texture_road);
	glBindTexture(GL_TEXTURE_2D, texture_road);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	free(image);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//EXPLOSION INITIALIZE
	glGenVertexArrays(1, &vertexArrayObject_2);
	// Set it as current, i.e., related calls will affect this object
	glBindVertexArray(vertexArrayObject_2);

	///////////////////////////////////////////////////////////////////////////
	// Create the positions buffer object
	///////////////////////////////////////////////////////////////////////////
	const float positions_2[] = {
		// X      Y       Z
		3.0f, 3.0f, -25.0f,  // v0
		3.0f, 10.0f, -25.0f, // v1
		10.0f,  10.0f, -25.0f, // v2
		10.0f,  3.0f, -25.0f,   // v3
		13.0f,  3.0f, -25.0f,   // v4
		13.0f,  13.0f, -25.0f   // v5
	};
	// Create a handle for the vertex position buffer
	glGenBuffers(1, &positionBuffer_2);
	// Set the newly created buffer as the current one
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer_2);
	// Send the vetex position data to the current buffer
	glBufferData(GL_ARRAY_BUFFER, labhelper::array_length(positions_2) * sizeof(float), positions_2,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false /*normalized*/, 0 /*stride*/, 0 /*offset*/);
	// Enable the attribute
	glEnableVertexAttribArray(0);
	///////////////////////////////////////////////////////////////////////////
	// Task 1 : Create the texture coordinates.
	//			Create the texture coordinates' buffer object.
	//			Set up the attrib pointer.
	//			Enable the vertex attrib array.
	///////////////////////////////////////////////////////////////////////////
	float texcoords_2[] = {
	0.0f, 0.0f,    // (u,v) for v0
	0.0f, 1.0f,   // (u,v) for v1
	1.0f, 1.0f,   // (u,v) for v2
	1.0f, 0.0f     // (u,v) for v3
	};
	glGenBuffers(1, &texCoordBuffer_2);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer_2);
	glBufferData(GL_ARRAY_BUFFER, labhelper::array_length(texcoords_2) * sizeof(float), texcoords_2, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, false/*normalized*/, 0/*stride*/, 0/*offset*/);
	glEnableVertexAttribArray(1);
	///////////////////////////////////////////////////////////////////////////
	// Create the element array buffer object
	///////////////////////////////////////////////////////////////////////////
	const int indices_2[] = {
		0, 2, 1, // Triangle 1
		0, 3, 2,  // Triangle 2
		2, 3, 4,  // Triangle 2
		2, 4, 5  // Triangle 2
	};
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, labhelper::array_length(indices_2) * sizeof(float), indices_2,
		GL_STATIC_DRAW);


	// The loadShaderProgram and linkShaderProgam functions are defined in glutil.cpp and
	// do exactly what we did in lab1 but are hidden for convenience
	shaderProgram = labhelper::loadShaderProgram("../lab2-textures/simple.vert",
		"../lab2-textures/simple.frag");

	//**********************************************

	//************************************
	//			Load Texture
	//************************************
	// Task 2
	int w_2, h_2, comp_2;
	unsigned char* image_2 = stbi_load("../scenes/textures/explosion.png", &w_2, &h_2, &comp_2, STBI_rgb_alpha);
	glGenTextures(1, &texture_exp);
	glBindTexture(GL_TEXTURE_2D, texture_exp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_2, h_2, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_2);
	free(image_2);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


///////////////////////////////////////////////////////////////////////////////
/// This function will be called once per frame, so the code to set up
/// the scene for rendering should go here
///////////////////////////////////////////////////////////////////////////////
void display(void)
{
	// The viewport determines how many pixels we are rasterizing to
	int w, h;
	SDL_GetWindowSize(g_window, &w, &h);
	// Set viewport
	glViewport(0, 0, w, h);

	// Set clear color
	glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
	// Clears the color buffer and the z-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// We disable backface culling for this tutorial, otherwise care must be taken with the winding order
	// of the vertices. It is however a lot faster to enable culling when drawing large scenes.
	//glDisable(GL_CULL_FACE); //
	glEnable(GL_CULL_FACE);
	// Disable depth testing
	glDisable(GL_DEPTH_TEST);
	// Set the shader program to use for this draw call
	glUseProgram(shaderProgram);

	// Set up a projection matrix
	float fovy = radians(45.0f);
	float aspectRatio = float(w) / float(h);
	float nearPlane = 0.01f;
	float farPlane = 400.0f;
	mat4 projectionMatrix = perspective(fovy, aspectRatio, nearPlane, farPlane);
	// Send it to the vertex shader
	int loc = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(loc, 1, false, &projectionMatrix[0].x);

	loc = glGetUniformLocation(shaderProgram, "cameraPosition");
	glUniform3f(loc, camera_pan, 10, 0);

	// Task 3.1glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_road);
	glBindVertexArray(vertexArrayObject);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//GUI mini
	switch (mini) {
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case 2:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case 5:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	//GUI mag
	switch (mag) {
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);

	//Draw explortion
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_exp);
	glBindVertexArray(vertexArrayObject_2);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glUseProgram(0); // "unsets" the current shader program. Not really necessary.
}


///////////////////////////////////////////////////////////////////////////////
/// This function is to hold the general GUI logic
///////////////////////////////////////////////////////////////////////////////
void gui()
{
	// ----------------- Set variables --------------------------
	ImGui::PushID("mag");
	ImGui::Text("Magnification");
	ImGui::RadioButton("GL_NEAREST", &mag, 0);
	ImGui::RadioButton("GL_LINEAR", &mag, 1);
	ImGui::PopID();

	ImGui::PushID("mini");
	ImGui::Text("Minification");
	ImGui::RadioButton("GL_NEAREST", &mini, 0);
	ImGui::RadioButton("GL_LINEAR", &mini, 1);
	ImGui::RadioButton("GL_NEAREST_MIPMAP_NEAREST", &mini, 2);
	ImGui::RadioButton("GL_NEAREST_MIPMAP_LINEAR", &mini, 3);
	ImGui::RadioButton("GL_LINEAR_MIPMAP_NEAREST", &mini, 4);
	ImGui::RadioButton("GL_LINEAR_MIPMAP_LINEAR", &mini, 5);
	ImGui::PopID();

	ImGui::SliderFloat("Anisotropic filtering", &anisotropy, 1.0, 16.0, "Number of samples: %.0f");
	ImGui::Dummy({ 0, 20 });
	ImGui::SliderFloat("Camera Panning", &camera_pan, -1.0, 1.0);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
		ImGui::GetIO().Framerate);
	// ----------------------------------------------------------

}

int main(int argc, char* argv[])
{
	g_window = labhelper::init_window_SDL("OpenGL Lab 2");

	initialize();

	// render-loop
	bool stopRendering = false;
	while (!stopRendering)
	{
		// Inform imgui of new frame
		ImGui_ImplSdlGL3_NewFrame(g_window);

		// check events (keyboard among other)
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// Allow ImGui to capture events.
			ImGui_ImplSdlGL3_ProcessEvent(&event);

			if (event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
			{
				stopRendering = true;
			}
			else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_g)
			{
				showUI = !showUI;
			}
			else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_PRINTSCREEN)
			{
				labhelper::saveScreenshot();
			}
		}

		// render to window
		display();

		// Render overlay GUI.
		if (showUI)
		{
			gui();
		}

		// Render the GUI.
		ImGui::Render();

		// Swap front and back buffer. This frame will now been displayed.
		SDL_GL_SwapWindow(g_window);
	}

	// Shut down everything. This includes the window and all other subsystems.
	labhelper::shutDown(g_window);
	return 0;
}
