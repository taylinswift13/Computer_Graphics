
#include <GL/glew.h>

#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <chrono>

#include <labhelper.h>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <Model.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;
using namespace labhelper;

using std::min;
using std::max;

///////////////////////////////////////////////////////////////////////////////
// Various globals
///////////////////////////////////////////////////////////////////////////////

// The window we'll be rendering to
SDL_Window* g_window = nullptr;

// Mouse input
ivec2 g_prevMouseCoords = { -1, -1 };
bool g_isMouseDragging = false;

float currentTime = 0.0f;
float deltaTime = 0.0f;
bool showUI = false;


///////////////////////////////////////////////////////////////////////////////
// Shader programs
///////////////////////////////////////////////////////////////////////////////
GLuint shaderProgram;


///////////////////////////////////////////////////////////////////////////////
// Scene objects and properties
///////////////////////////////////////////////////////////////////////////////

// Models
Model* cityModel = nullptr;
Model* carModel = nullptr;
Model* groundModel = nullptr;
mat4 carModelMatrix(1.0f), car_2ModelMatrix(1.0f);

vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);

// Camera parameters
vec3 cameraPosition(15.0f, 15.0f, 15.0f);
vec3 cameraDirection(-1.0f, -1.0f, -1.0f);
mat4 T(1.0f), R(1.0f), T2(1.0f), R2(1.0f), T3(1.0f), S(1.0F), L(1.0f);

struct PerspectiveParams
{
	float fov;
	int w;
	int h;
	float near;
	float far;
};
PerspectiveParams pp = { 45.0f, 1280, 720, 0.1f, 300.0f };
bool pers = false;
int old_w = 1280;
int old_h = 720;


///////////////////////////////////////////////////////////////////////////
// Load models (both vertex buffers and textures).
///////////////////////////////////////////////////////////////////////////
void loadModels()
{
	cityModel = loadModelFromOBJ("../scenes/city.obj");
	carModel = loadModelFromOBJ("../scenes/car.obj");
	groundModel = loadModelFromOBJ("../scenes/ground_plane.obj");
}


///////////////////////////////////////////////////////////////////////////////
/// This function is called once at the start of the program and never again
///////////////////////////////////////////////////////////////////////////////
void initialize()
{
	ENSURE_INITIALIZE_ONLY_ONCE();

	// Load shader program
	shaderProgram = labhelper::loadShaderProgram("../lab3-camera/simple.vert", "../lab3-camera/simple.frag");

	// Load models
	loadModels();
}

void drawGround(mat4 mvpMatrix)
{
	mat4 mm = glm::translate(vec3(0, -0.5 + 0.0005, 0));
	mvpMatrix = mvpMatrix * mm;
	int mvploc = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");
	glUniformMatrix4fv(mvploc, 1, false, &mvpMatrix[0].x);
	int mloc = glGetUniformLocation(shaderProgram, "modelMatrix");
	glUniformMatrix4fv(mloc, 1, false, &mm[0].x);
	render(groundModel);
}


///////////////////////////////////////////////////////////////////////////////
/// This function will be called once per frame, so the code to set up
/// the scene for rendering should go here
///////////////////////////////////////////////////////////////////////////////
void display()
{
	// Set up
	int w, h;
	SDL_GetWindowSize(g_window, &w, &h);

	if (pp.w != old_w || pp.h != old_h)
	{
		SDL_SetWindowSize(g_window, pp.w, pp.h);
		w = pp.w;
		h = pp.h;
		old_w = pp.w;
		old_h = pp.h;
	}

	glViewport(0, 0, w, h);                             // Set viewport
	glClearColor(0.2f, 0.2f, 0.8f, 1.0f);               // Set clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the color buffer and the z-buffer
	glEnable(GL_DEPTH_TEST);                            // enable Z-buffering
	glDisable(GL_CULL_FACE);                            // disables not showing back faces of triangles

	// Set the shader program to use for this draw call
	glUseProgram(shaderProgram);

	// Set up model matrices
	mat4 cityModelMatrix(1.0f);

	// Set up the view matrix
	// The view matrix defines where the viewer is looking
	// Initially fixed, but will be replaced in the tutorial.
	/*mat4 constantViewMatrix = mat4(0.707106769f, -0.408248276f, 1.00000000f, 0.000000000f,  //
		0.000000000f, 0.816496551f, 1.00000000f, 0.000000000f,   //
		-0.707106769f, -0.408248276f, 1.00000000f, 0.000000000f, //
		0.000000000f, 0.000000000f, -30.0000000f, 1.00000000f);  //*/

		// use camera direction as -z axis and compute the x (cameraRight) and y (cameraUp) base vectors
	vec3 cameraRight = normalize(cross(cameraDirection, worldUp));
	vec3 cameraUp = normalize(cross(cameraRight, cameraDirection));

	mat3 cameraBaseVectorsWorldSpace(cameraRight, cameraUp, -cameraDirection);

	mat4 cameraRotation = mat4(transpose(cameraBaseVectorsWorldSpace));
	mat4 viewMatrix;
	if (pers)//FIRST PERSON
	{
		viewMatrix = T * R;
	}
	else //Third Person
	{
		viewMatrix = cameraRotation * translate(-cameraPosition);
	}
	// Setup the projection matrix
	if (w != old_w || h != old_h)
	{
		pp.h = h;
		pp.w = w;
		old_w = w;
		old_h = h;
	}
	mat4 projectionMatrix = perspective(radians(pp.fov), float(pp.w) / float(pp.h), pp.near, pp.far);


	const int mvploc = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");
	const int mloc = glGetUniformLocation(shaderProgram, "modelMatrix");

	// Concatenate the three matrices and pass the final transform to the vertex shader

	// City
	mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * cityModelMatrix;
	glUniformMatrix4fv(mvploc, 1, false, &modelViewProjectionMatrix[0].x);
	glUniformMatrix4fv(mloc, 1, false, &cityModelMatrix[0].x);
	render(cityModel);

	// Ground
	// Task 5: Uncomment this
	drawGround(modelViewProjectionMatrix);

	// car
	carModelMatrix = T * R;
	modelViewProjectionMatrix = projectionMatrix * viewMatrix * carModelMatrix;
	glUniformMatrix4fv(mvploc, 1, false, &modelViewProjectionMatrix[0].x);
	glUniformMatrix4fv(mloc, 1, false, &carModelMatrix[0].x);
	render(carModel);

	T2 = translate(mat4(1.0f), vec3(25.0f, 0.0f, 0.0f));
	R2 = rotate(mat4(1.0f), currentTime, vec3(0.0f, 1.0f, 0.0f));
	T3 = translate(mat4(1.0f), vec3(25.0f, 0.0f, 0.0f));

	S = scale(mat4(1.0f), vec3(1.5f, 1.5f, 1.5f));

	car_2ModelMatrix = T2 * R2 * T3 * S;
	modelViewProjectionMatrix = projectionMatrix * viewMatrix * car_2ModelMatrix;
	glUniformMatrix4fv(mvploc, 1, false, &modelViewProjectionMatrix[0].x);
	glUniformMatrix4fv(mloc, 1, false, &car_2ModelMatrix[0].x);

	render(carModel);

	glUseProgram(0);
}


///////////////////////////////////////////////////////////////////////////////
/// This function is used to update the scene according to user input
///////////////////////////////////////////////////////////////////////////////
bool handleEvents(void)
{
	// check new events (keyboard among other)
	SDL_Event event;
	bool quitEvent = false;

	while (SDL_PollEvent(&event))
	{
		// Allow ImGui to capture events.
		ImGui_ImplSdlGL3_ProcessEvent(&event);

		// More info at https://wiki.libsdl.org/SDL_Event
		if (event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
		{
			quitEvent = true;
		}
		else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_g)
		{
			showUI = !showUI;
		}
		else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_PRINTSCREEN)
		{
			labhelper::saveScreenshot();
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			g_isMouseDragging = true;
			int x;
			int y;
			SDL_GetMouseState(&x, &y);
			g_prevMouseCoords.x = x;
			g_prevMouseCoords.y = y;

		}
		if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
		{
			g_isMouseDragging = false;
		}

		if (event.type == SDL_MOUSEMOTION && g_isMouseDragging && !(ImGui::GetIO().WantCaptureMouse))
		{
			// More info at https://wiki.libsdl.org/SDL_MouseMotionEvent
			int delta_x = event.motion.x - g_prevMouseCoords.x;
			int delta_y = event.motion.y - g_prevMouseCoords.y;
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				float rotationSpeed = 0.005f;
				mat4 yaw = rotate(rotationSpeed * -delta_x, worldUp);
				mat4 pitch = rotate(rotationSpeed * -delta_y, normalize(cross(cameraDirection, worldUp)));
				cameraDirection = vec3(pitch * yaw * vec4(cameraDirection, 0.0f));
			}
			g_prevMouseCoords.x = event.motion.x;
			g_prevMouseCoords.y = event.motion.y;
		}
	}

	// check keyboard state (which keys are still pressed)
	const uint8_t* state = SDL_GetKeyboardState(nullptr);
	const float speed = 10.f;
	vec4 car_forward = R * vec4(0, 0, 1, 0);
	const float rotateSpeed = 2.f;

	// implement camera controls based on key states
	if (state[SDL_SCANCODE_UP])
	{
		T = glm::translate(vec3(car_forward) * speed * deltaTime) * T;

		printf("Key Up is pressed down\n");
	}
	if (state[SDL_SCANCODE_DOWN])
	{
		T = glm::translate(vec3(-car_forward) * speed * deltaTime) * T;
		printf("Key Down is pressed down\n");
	}
	if (state[SDL_SCANCODE_LEFT])
	{
		R = glm::rotate(rotateSpeed * deltaTime, glm::vec3(0, 1, 0)) * R;

		printf("Key Left is pressed down\n");
	}
	if (state[SDL_SCANCODE_RIGHT])
	{
		R = glm::rotate(-rotateSpeed * deltaTime, glm::vec3(0, 1, 0)) * R;
		printf("Key Right is pressed down\n");
	}
	if (state[SDL_SCANCODE_W])
	{
		float translateDelta = 0.1f;
		cameraPosition += cameraDirection * translateDelta;
	}
	if (state[SDL_SCANCODE_S])
	{
		float translateDelta = 0.1f;
		cameraPosition -= cameraDirection * translateDelta;
	}
	if (state[SDL_SCANCODE_A])
	{
		float translateDelta = 0.1f;
		vec3 cameraDirectionP = perpendicular(cameraDirection);
		cameraPosition -= cameraDirectionP * translateDelta;
	}
	if (state[SDL_SCANCODE_D])
	{
		float translateDelta = 0.1f;
		vec3 cameraDirectionP = perpendicular(cameraDirection);
		cameraPosition += cameraDirectionP * translateDelta;
	}

	return quitEvent;
}


///////////////////////////////////////////////////////////////////////////////
/// This function is to hold the general GUI logic
///////////////////////////////////////////////////////////////////////////////
void gui()
{
	// ----------------- Set variables --------------------------
	ImGui::SliderFloat("Field Of View", &pp.fov, 1.0f, 180.0f, "%.0f");
	ImGui::SliderInt("Width", &pp.w, 256, 1920);
	ImGui::SliderInt("Height", &pp.h, 256, 1080);
	ImGui::Text("Aspect Ratio: %.2f", float(pp.w) / float(pp.h));
	ImGui::SliderFloat("Near Plane", &pp.near, 0.1f, 300.0f, "%.2f", 2.f);
	ImGui::SliderFloat("Far Plane", &pp.far, 0.1f, 300.0f, "%.2f", 2.f);
	ImGui::Checkbox("First Person", &pers);
	if (ImGui::Button("Reset"))
	{
		pp.fov = 45.0f;
		pp.w = 1280;
		pp.h = 720;
		pp.near = 0.1f;
		pp.far = 300.0f;
	}
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
		ImGui::GetIO().Framerate);
	// ----------------------------------------------------------
}


int main(int argc, char* argv[])
{
	g_window = labhelper::init_window_SDL("OpenGL Lab 3");

	initialize();

	// render-loop
	bool stopRendering = false;
	auto startTime = std::chrono::system_clock::now();

	while (!stopRendering)
	{
		// update currentTime
		std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
		deltaTime = timeSinceStart.count() - currentTime;
		currentTime = timeSinceStart.count();

		// Inform imgui of new frame
		ImGui_ImplSdlGL3_NewFrame(g_window);

		stopRendering = handleEvents();

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
