#include <malloc.h>
#include <stdio.h>
#include <assert.h>

#include <vector>
#include <chrono>
#include <string>

#include "Shader.hpp"
#include "GenerateGeometries.hpp"

#define WIDTH 1200
#define HEIGHT 900
#define NAME "Simple Geometry Rendering"


struct Shaders {
	std::unique_ptr<Shader> phong{nullptr};
	std::unique_ptr<Shader> norm{nullptr};
};

#define UNUSED(V) ((void)V)

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
	

	int version = gladLoadGLLoader(SDL_GL_GetProcAddress);
	if (version == 0)
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
	
	
	Shaders shaders;
	const auto phong_vert = Shader::file_slurp("../phong.vert");
	if (!phong_vert.has_value())
		throw std::runtime_error("could not load phong vert");

	const auto phong_frag = Shader::file_slurp("../phong.frag");
	if (!phong_vert.has_value())
		throw std::runtime_error("could not load phong frag");

	auto phong_shader = Shader::create(phong_vert.value().c_str(), phong_frag.value().c_str());
	std::visit(variant_switch {
			[] (const Shader::Error& error) {
				throw std::runtime_error(error.what);
			},
			[&] (Shader::Uptr& ptr) {
				shaders.phong = std::move(ptr);
			}
		}, phong_shader);
	
	const auto norm_vert = Shader::file_slurp("../normal.vert");
	if (!norm_vert.has_value())
		throw std::runtime_error("could not load normal.vert");

	const auto norm_frag = Shader::file_slurp("../normal.frag");
	if (!norm_vert.has_value())
		throw std::runtime_error("could not load normal.frag");

	auto norm_shader = Shader::create(norm_vert.value().c_str(), norm_frag.value().c_str());
	std::visit(variant_switch {
			[] (const Shader::Error& error) {
				throw std::runtime_error(error.what);
			},
			[&] (Shader::Uptr& ptr) {
				shaders.norm = std::move(ptr);
			}
		}, norm_shader);

	RandomRotatedMesh<NormMesh> norm_cube{
		NormMesh::cube(),
		RandomModelRotator{}};

	RandomRotatedMesh<IndexedNormMesh> norm_sphere{
		IndexedNormMesh::sphere(),
		RandomModelRotator{}};

	glm::mat4 proj = glm::perspective(glm::radians(45.0f),
									  (float)WIDTH/(float)HEIGHT,
									  0.1f,
									  100.0f);

	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), 
								 glm::vec3(0.0f, 0.0f, 0.0f), 
								 glm::vec3(0.0f, 1.0f, 0.0f));

	auto startTime = std::chrono::high_resolution_clock::now();
	
	bool exit = false;
	SDL_Event event;
	while (!exit) {
		/** =======================================================
		 * Handle SDL2 Inputs
		 */
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				exit = true;
				break;
				
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ) {
				case SDLK_ESCAPE:
					exit = true;
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
		const auto currentTime = std::chrono::high_resolution_clock::now();
		const float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		/** ***************************************************
		 * Draw
		 */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GL_THROW_ON_ERROR();

		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		GL_THROW_ON_ERROR();

		glBindVertexArray(norm_cube.mesh.vao);
		shaders.norm->activate();
		glBindBuffer(GL_ARRAY_BUFFER, norm_cube.mesh.vbo);
		shaders.norm->set_mat4("view", view);
		shaders.norm->set_mat4("proj", proj);
		shaders.norm->set_mat4("model", norm_cube.rotator.final_model_matrix(time));
		GL_THROW_ON_ERROR();
		glDrawArrays(GL_TRIANGLES, 0, norm_cube.mesh.length);
		GL_THROW_ON_ERROR();
		
		glBindVertexArray(norm_sphere.mesh.vao);
		shaders.norm->activate();
		glBindBuffer(GL_ARRAY_BUFFER, norm_sphere.mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, norm_sphere.mesh.ebo);
		shaders.norm->set_mat4("view", view);
		shaders.norm->set_mat4("proj", proj);
		shaders.norm->set_mat4("model", norm_sphere.rotator.final_model_matrix(time));
		GL_THROW_ON_ERROR();
		glDrawElements(GL_TRIANGLES, norm_sphere.mesh.indice_length, GL_UNSIGNED_INT, 0);
		GL_THROW_ON_ERROR();


		SDL_GL_SwapWindow(window);
		GL_THROW_ON_ERROR();
	}

}
