#include "utils/Shader.hpp"
#include "utils/OrbitCamera.hpp"
#include "utils/DeltaClock.hpp"

#include "GenerateGeometries.hpp"
#include "Renderers.hpp"

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

enum class DrawType {
	NormalDirection,
	Wireframe,
	Phong
};

[[nodiscard]]
DrawType next_draw_type(const DrawType type)
{
	if (type == DrawType::NormalDirection)
		return DrawType::Wireframe;
	else if (type == DrawType::Wireframe)
		return DrawType::Phong;
	else
		return DrawType::NormalDirection;
}

struct RandomScene
{
	DrawType draw_type = DrawType::Phong;

	std::vector<Drawable> scattered_drawables = {
		RandomRotated<NormMesh>(cube(sg_material_gold()), RandomModelRotator{}),
		RandomRotated<NormMesh>(cube(sg_material_obsidian()), RandomModelRotator{}),
		RandomRotated<NormMesh>(cube(sg_material_default_flat_white()), RandomModelRotator{}), RandomRotated<NormMesh>(cube(sg_material_ruby()), RandomModelRotator{}),
		RandomRotated<NormMesh>(cube(sg_material_emerald()), RandomModelRotator{}),
		
		RandomRotated<IndexedNormMesh>(sphere(sg_material_gold()), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(sg_material_obsidian()), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(sg_material_default_flat_white()), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(sg_material_ruby()), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(sg_material_emerald()), RandomModelRotator{}),
	};

	glm::mat4 floor_model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)), glm::vec3(10.0f, 0.05f, 10.0f));
	Drawable floor = Model<NormMesh>(cube(sg_material_default_flat_white()), floor_model);

	glm::mat4 hill_model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, -3.0f)), glm::vec3(5.0f));
	Drawable hill = Model<IndexedNormMesh>(sphere(sg_material_emerald()), hill_model);
	
};

enum class InspectedObject {
	Sphere,
	Cube,
};

InspectedObject next_inspected_object(InspectedObject object)
{
	switch (object) {
	case InspectedObject::Sphere: return InspectedObject::Cube;
	case InspectedObject::Cube:   return InspectedObject::Sphere;
	default:                      return InspectedObject::Cube;
	}
}

struct InspectScene
{
	DrawType draw_type = DrawType::Phong;
	InspectedObject object = InspectedObject::Cube;

	Drawable dcube = Model<NormMesh>(cube(sg_material_default_flat_white()), glm::mat4(1.0f));
	Drawable dsphere = Model<IndexedNormMesh>(sphere(sg_material_default_flat_white()), glm::mat4(1.0f));
};

using Scene = std::variant<RandomScene, InspectScene>;

Scene next_scene(Scene scene)
{
	if (auto* p = std::get_if<RandomScene>(&scene)) {
		InspectScene newscene{};
		newscene.draw_type = p->draw_type;
		return newscene;
	}
	else if (auto* p = std::get_if<InspectScene>(&scene)) {
		RandomScene newscene{};
		newscene.draw_type = p->draw_type;
		return newscene;
	}
	else {
		throw std::runtime_error("Unknown scene type");
	}
}

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
	DeltaClock deltaclock;
	
	auto camera = std::make_unique<OrbitCamera>(glm::vec3(0.0f), 10.0f);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
											(float)WIDTH/(float)HEIGHT,
											0.1f,
											100.0f);
	
	NormalDirectionDrawer normal_direction_drawer{};
	WireframeDrawer wireframe_drawer{};
	PhongDrawer phong_drawer{};
	Scene current_scene = InspectScene{};


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
				case SDLK_n:
					if (auto* p = std::get_if<InspectScene>(&current_scene)) {
						p->draw_type = next_draw_type(p->draw_type);
					}
					if (auto* p = std::get_if<RandomScene>(&current_scene)) {
						p->draw_type = next_draw_type(p->draw_type);
					}
					break;
				case SDLK_m:
					current_scene = next_scene(current_scene);
					break;
				case SDLK_b:
					if (auto* p = std::get_if<InspectScene>(&current_scene))
					p->object = next_inspected_object(p->object);
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
		glEnable(GL_DEPTH_TEST);  
	
		if (auto* p = std::get_if<RandomScene>(&current_scene)) {
			const auto draw_world = [&] (auto drawer) 
			{
				std::visit(drawer, p->floor);
				std::visit(drawer, p->hill);
				
				for (auto& drawable: p->scattered_drawables)
					std::visit(drawer, drawable);
			};
			
			switch (p->draw_type) {
			case DrawType::NormalDirection: {
				draw_world(normal_direction_drawer(camera->view(), projection, totaltime));
				break;
			}
			case DrawType::Wireframe: {
				const auto red = glm::vec3(1.0f, 0.0f, 0.0f);
				draw_world(wireframe_drawer(camera->view(),
											projection,
											totaltime,
											red));
				break;
			}
			case DrawType::Phong: {
				const std::vector<PointLight> pointlights{
					PointLight(glm::vec3(0.0f, 1.0f, 0.0f), sg_material_ruby()),
				};
				const auto directionallight = DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f),
															   sg_material_default_flat_white());
				
				draw_world(phong_drawer(camera->view(),
										projection,
										totaltime,
										directionallight,
										pointlights));
				break;
			}
			default: 
				break;
			}
		}
		else if (auto* p = std::get_if<InspectScene>(&current_scene)) {
			
			auto draw_inspected = [&] (auto drawer) {
				switch (p->object) {
				case InspectedObject::Cube:
					std::visit(drawer, p->dcube);
					break;
				case InspectedObject::Sphere:
					std::visit(drawer, p->dsphere);
					break;
				default: break;
			}

			};


			switch (p->draw_type) {
			case DrawType::NormalDirection: {
				draw_inspected(normal_direction_drawer(camera->view(), projection, totaltime));
				break;
			}
			case DrawType::Wireframe: {
				const auto red = glm::vec3(1.0f, 0.0f, 0.0f);
				draw_inspected(wireframe_drawer(camera->view(),
												projection,
												totaltime,
												red));
				break;
			}
			case DrawType::Phong: {
				const std::vector<PointLight> pointlights{
					PointLight(glm::vec3(0.0f, 1.0f, 0.0f), sg_material_ruby()),
				};
				const auto directionallight = DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f),
															   sg_material_default_flat_white());
				
				draw_inspected(phong_drawer(camera->view(),
											projection,
											totaltime,
											directionallight,
											pointlights));
				break;
			}
			default: 
				break;
			}

		}
	
		SDL_GL_SwapWindow(window);
		GL_THROW_ON_ERROR();

		deltaclock.tick();
	}
}
