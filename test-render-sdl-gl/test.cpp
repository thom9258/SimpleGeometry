#include "utils/Shader.hpp"
#include "utils/OrbitCamera.hpp"
#include "utils/DeltaClock.hpp"

#include "GenerateGeometries.hpp"

#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <chrono>
#include <string>


#define WIDTH 1200
#define HEIGHT 900
#define NAME "Simple Geometry Rendering"

#define UNUSED(V) ((void)V)

//template<class... Ts> struct variant_switch : Ts... { using Ts::operator()...; };
//template<class... Ts> variant_switch(Ts...) -> variant_switch<Ts...>;

template <typename Mesh>
struct RandomRotated {
	explicit RandomRotated(const Mesh mesh, const RandomModelRotator rotator)
		: mesh(mesh), rotator(rotator) 
	{}
	Mesh mesh;
	RandomModelRotator rotator;
};

template <typename Mesh>
struct Model {
	explicit Model(const Mesh mesh, const glm::mat4 model)
		: mesh(mesh), model(model) 
	{}
	Mesh mesh;
	glm::mat4 model;
};

using Drawable = std::variant<RandomRotated<NormMesh>,
							  RandomRotated<IndexedNormMesh>,
							  Model<NormMesh>,
							  Model<IndexedNormMesh>
							  >;


int main(int argc, char** argv)
{
	UNUSED(argc);
	UNUSED(argv);
	srand(static_cast<unsigned>(time(0)));

	/** ************************************************************************'
	 *  Setup SDL2 Context and Load OpenGL 3.1 using GLAD
	 */
	auto sdl_status = SDL_Init(SDL_INIT_VIDEO);
	if (sdl_status < 0)
		return 1;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	auto window = SDL_CreateWindow(NAME,
								   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
								   WIDTH, HEIGHT,
								   SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if (!window)
		throw std::runtime_error("Could not create Window! SDL Error: "
								 + std::string(SDL_GetError()));
	
	auto context = SDL_GL_CreateContext(window);
	assert(context && "Unable to create SDL2 Context");
	if (!context)
		throw std::runtime_error("Could not create Context! SDL Error: "
								 + std::string(SDL_GetError()));
	
	if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
		throw std::runtime_error("Unable to load GL");
	
	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));
	
	sdl_status = SDL_GL_SetSwapInterval(1);
	if(sdl_status < 0)
		throw std::runtime_error("Unable to set VSync! SDL Error: "
								 + std::string(SDL_GetError()));
	
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	if (glGetError() != GL_NO_ERROR)
		throw std::runtime_error("Unable to Set Viewport and Clearcolor");
	
	/** ***************************************************************
	 *  Rendering Initialization
	 */
	Shaders shaders;
	DeltaClock deltaclock;
	
	auto camera = std::make_unique<OrbitCamera>(glm::vec3(0.0f), 10.0f);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
											(float)WIDTH/(float)HEIGHT,
											0.1f,
											100.0f);

	std::vector<Drawable> scattered_drawables = {
		RandomRotated<NormMesh>(cube(), RandomModelRotator{}),
		RandomRotated<NormMesh>(cube(), RandomModelRotator{}),
		RandomRotated<NormMesh>(cube(), RandomModelRotator{}),
		RandomRotated<NormMesh>(cube(), RandomModelRotator{}),

		RandomRotated<IndexedNormMesh>(sphere(), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(), RandomModelRotator{}),
	};

	auto floor_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	floor_model = glm::scale(floor_model, glm::vec3(10.0f, 0.05f, 10.0f));
	Drawable floor = Model<NormMesh>(cube(), floor_model);

	auto hill_model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, -3.0f));
	hill_model = glm::scale(hill_model, glm::vec3(5.0f));
	Drawable hill = Model<IndexedNormMesh>(sphere(), hill_model);
	
	bool exit = false;
	while (!exit) {
		const auto deltatime = deltaclock.deltatime_ms();
		const auto totaltime = deltaclock.totaltime_ms();
		const float movespeed = 3.0f * deltatime;

		/** =======================================================
		 * Handle SDL2 Inputs
		 */
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				exit = true;
				break;
				
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					exit = true;
					break;
				case SDLK_w:
					camera->add_rotation(-movespeed, 0.0f);
					break;
				case SDLK_s:
					camera->add_rotation(movespeed, 0.0f);
					break;
				case SDLK_a:
					camera->add_rotation(0.0f, movespeed);
					break;
				case SDLK_d:
					camera->add_rotation(0.0f, -movespeed);
					break;
				case SDLK_DOWN:
					camera->add_radius(movespeed*2);
					break;
				case SDLK_UP:
					camera->add_radius(-movespeed*2);
					break;

				}
				break;
				
			case SDL_WINDOWEVENT: {
				const SDL_WindowEvent& wev = event.window;
				switch (wev.event) {
				case SDL_WINDOWEVENT_CLOSE:
					exit = true;
					break;
				}
			} break;
			}
		}

		/** ***************************************************
		 * Draw
		 */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GL_THROW_ON_ERROR();

		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		GL_THROW_ON_ERROR();
		
		auto drawer = variant_switch {
				[&] (RandomRotated<NormMesh>& draw) {
					glBindVertexArray(draw.mesh.vao);
					shaders.norm->activate();
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					shaders.norm->set_mat4("view", camera->view());
					shaders.norm->set_mat4("proj", projection);
					shaders.norm->set_mat4("model", draw.rotator.final_model_matrix(totaltime));
					GL_THROW_ON_ERROR();
					glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
					GL_THROW_ON_ERROR();
				},
				[&] (RandomRotated<IndexedNormMesh>& draw) {
					glBindVertexArray(draw.mesh.vao);
					shaders.norm->activate();
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
					shaders.norm->set_mat4("view", camera->view());
					shaders.norm->set_mat4("proj", projection);
					shaders.norm->set_mat4("model", draw.rotator.final_model_matrix(totaltime));
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				},
				[&] (Model<NormMesh>& draw) {
					glBindVertexArray(draw.mesh.vao);
					shaders.norm->activate();
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					shaders.norm->set_mat4("view", camera->view());
					shaders.norm->set_mat4("proj", projection);
					shaders.norm->set_mat4("model", draw.model);
					GL_THROW_ON_ERROR();
					glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
					GL_THROW_ON_ERROR();
				},
				[&] (Model<IndexedNormMesh>& draw) {
					glBindVertexArray(draw.mesh.vao);
					shaders.norm->activate();
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
					shaders.norm->set_mat4("view", camera->view());
					shaders.norm->set_mat4("proj", projection);
					shaders.norm->set_mat4("model", draw.model);
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				},

				[] (auto&) {
					throw std::runtime_error("could not handle drawable type..");
				}
		};
		

		std::visit(drawer, floor);
		std::visit(drawer, hill);

		for (auto& drawable: scattered_drawables)
			std::visit(drawer, drawable);
		

		SDL_GL_SwapWindow(window);
		GL_THROW_ON_ERROR();

		deltaclock.tick();
	}
}
