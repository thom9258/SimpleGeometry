#include "utils/Shader.hpp"
#include "utils/OrbitCamera.hpp"
#include "utils/DeltaClock.hpp"

#include "Geometry.hpp"
#include "Renderers.hpp"

#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <chrono>
#include <string>

using namespace std;

size_t constexpr window_width = 1200;
size_t constexpr window_height = 900;
string_view constexpr window_name = "Simple Geometry Rendering";

enum class DrawType 
{
	Phong,
	Wireframe,
	PhongAndWireframe,
	NormalColor,
	PhongNormalWireframe
};

[[nodiscard]]
auto next(DrawType type)
	-> DrawType
{
	switch (type) {
	case DrawType::Phong:                return DrawType::Wireframe;
	case DrawType::Wireframe:            return DrawType::PhongAndWireframe;
	case DrawType::PhongAndWireframe:    return DrawType::NormalColor;
	case DrawType::NormalColor:          return DrawType::PhongNormalWireframe;
	case DrawType::PhongNormalWireframe: return DrawType::Phong;
	default:                             return DrawType::Phong;
	};
}

auto operator<<(std::ostream& os, DrawType type)
	-> std::ostream&
{
	switch (type) {
	case DrawType::Phong:                os << "Phong"; break;
	case DrawType::Wireframe:            os << "Wireframe"; break;
	case DrawType::PhongAndWireframe:    os << "PhongAndWireframe"; break;
	case DrawType::NormalColor:          os << "NormalColor"; break;
	case DrawType::PhongNormalWireframe: os << "PhongNormalWireframe"; break;
	};

	return os;
}

enum class Object 
{
	Sphere,
	Cube,
	Cylinder,
	GizmoCone,
	GizmoSphere,
	GizmoCube,
	GizmoCapsule,
};

[[nodiscard]]
auto next(Object object)
	-> Object
{
	switch (object) {
	case Object::Sphere:      return Object::Cube;
	case Object::Cube:        return Object::Cylinder;
	case Object::Cylinder:    return Object::GizmoCone;
	case Object::GizmoCone:   return Object::GizmoSphere;
	case Object::GizmoSphere:   return Object::GizmoCapsule;
	case Object::GizmoCapsule: return Object::Sphere;
	case Object::GizmoCube: return Object::Sphere;
	}
	
	return Object::Sphere;
}

auto operator<<(std::ostream& os, Object object)
	-> std::ostream&
{
	switch (object) {
	case Object::Sphere:      os << "Sphere"; break;
	case Object::Cube:        os << "Cube"; break;
	case Object::Cylinder:    os << "Cylinder"; break;
	case Object::GizmoCone:   os << "GizmoCone"; break;
	case Object::GizmoSphere: os << "GizmoSphere"; break;
	case Object::GizmoCube:   os << "GizmoCube"; break;
	case Object::GizmoCapsule:   os << "GizmoCapsule"; break;
	}
	return os;
}

void print_state(Object object, DrawType draw_type)
{
	std::cout << object << " : "<< draw_type << std::endl;
}

int main(int argc, char** argv)
{
	static_cast<void>(argc);
	static_cast<void>(argv);

	/** ************************************************************************'
	 *  Setup SDL2 Context and Load OpenGL 3.1 using GLAD
	 */
	auto sdl_status = SDL_Init(SDL_INIT_VIDEO);
	if (sdl_status < 0) {
		throw std::runtime_error("Could not init SDL2");
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window = SDL_CreateWindow(window_name.data(),
										  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
										  window_width, window_height,
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
	
	glViewport(0, 0, window_width, window_height);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	if (glGetError() != GL_NO_ERROR)
		throw std::runtime_error("Unable to Set Viewport and Clearcolor");
	
	/** ***************************************************************
	 *  Rendering Initialization
	 */
	DeltaClock deltaclock;
	
	float constexpr orbit_radius = 5.0f;
	glm::vec3 constexpr orbit_target = glm::vec3(0.0f);
	OrbitCamera camera(orbit_target, orbit_radius);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
											(float)window_width / (float)window_height,
											0.1f,
											100.0f);

	NormalDirectionDrawer normalcolor_drawer{};
	WireframeDrawer wireframe_drawer{};
	PhongDrawer phong_drawer{};
	
	Object object = Object::GizmoCapsule;
	DrawType draw_type = DrawType::Wireframe;
	print_state(object, draw_type);
	bool exit = false;

	while (!exit) {
		auto const deltatime = deltaclock.deltatime_ms();
		auto const totaltime = deltaclock.totaltime_ms();
		float const movespeed = 3.0f * deltatime;
		float const zoomspeed = movespeed * 2;

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
					draw_type = next(draw_type);
					print_state(object, draw_type);
					break;
				case SDLK_m:
					object = next(object);
					print_state(object, draw_type);
					break;

				case SDLK_w:
					camera.add_rotation(-movespeed, 0.0f);
					break;
				case SDLK_s:
					camera.add_rotation(movespeed, 0.0f);
					break;
				case SDLK_a:
					camera.add_rotation(0.0f, movespeed);
					break;
				case SDLK_d:
					camera.add_rotation(0.0f, -movespeed);
					break;
				case SDLK_q:
					camera.add_radius(zoomspeed);
					break;
				case SDLK_e:
					camera.add_radius(-zoomspeed);
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
		
		sg_material const white_material = sg_material_default_flat_white();
		sg_material const ruby_material = sg_material_default_flat_white();
		PointLight pointlight(glm::vec3(0.0f, 1.0f, 0.0f), ruby_material);
		pointlight.material.ambient = glm::vec3(0.0f);
		pointlight.material.diffuse = glm::vec3(0.0f);
		pointlight.material.specular = glm::vec3(0.0f);
		pointlight.material.shininess = 0.0f;

		DirectionalLight const directionallight(glm::vec3(-0.2f, -1.0f, -0.3f), ruby_material);
		glm::mat4 const model(1.0f);

		glm::vec3 const wireframe_red(1.0f, 0.0f, 0.0f);
		glm::vec3 const wireframe_blue(0.0f, 0.0f, 1.0f);
		float const normalvector_length = 0.3f;

		std::vector<Drawable> drawables;
		std::vector<Drawable> normalvector_drawables;
		
		if (object == Object::Sphere) {
			auto [vertices, indices] = create_sphere();
			IndexedNormMesh mesh = create_mesh(vertices, indices, ruby_material);
			drawables.push_back(Model<IndexedNormMesh>{mesh, model});
			
			std::vector<VertexPosNorm> normalvectors = create_normalvectors(vertices,
																			indices,
																			normalvector_length);
			NormMesh normalvector_mesh = create_mesh(normalvectors, white_material);
			normalvector_drawables.push_back(Model<NormMesh>{normalvector_mesh, model});

		}
		else if (object == Object::Cube) {
			std::vector<VertexPosNorm> vertices = create_cube();
			NormMesh mesh = create_mesh(vertices, white_material);
			drawables.push_back(Model<NormMesh>{mesh, model});
			
			std::vector<VertexPosNorm> normalvectors = create_normalvectors(vertices,
																			normalvector_length);
			NormMesh normalvector_mesh = create_mesh(normalvectors, white_material);
			normalvector_drawables.push_back(Model<NormMesh>{normalvector_mesh, model});
		}
		else if (object == Object::Cylinder) {
			std::vector<VertexPosNorm> vertices = create_cylinder();
			NormMesh mesh = create_mesh(vertices, white_material);
			drawables.push_back(Model<NormMesh>{mesh, model});
			
			std::vector<VertexPosNorm> normalvectors = create_normalvectors(vertices,
																			normalvector_length);
			NormMesh normalvector_mesh = create_mesh(normalvectors, white_material);
			normalvector_drawables.push_back(Model<NormMesh>{normalvector_mesh, model});
		}
		else if (object == Object::GizmoCone) {
			std::vector<VertexPosNorm> vertices = create_gizmo_cone();
			NormMesh mesh = create_mesh(vertices, ruby_material);
			drawables.push_back(Model<NormMesh>{mesh, model});
			normalvector_drawables.push_back(Model<NormMesh>{mesh, model});
		}
		else if (object == Object::GizmoSphere) {
			std::vector<VertexPosNorm> vertices = create_gizmo_sphere();
			NormMesh mesh = create_mesh(vertices, ruby_material);
			drawables.push_back(Model<NormMesh>{mesh, model});
			normalvector_drawables.push_back(Model<NormMesh>{mesh, model});
		}
		else if (object == Object::GizmoCube) {
			std::vector<VertexPosNorm> vertices = create_gizmo_cube();
			NormMesh mesh = create_mesh(vertices, ruby_material);
			drawables.push_back(Model<NormMesh>{mesh, model});
			normalvector_drawables.push_back(Model<NormMesh>{mesh, model});
		}
		else if (object == Object::GizmoCapsule) {
			std::vector<VertexPosNorm> vertices = create_gizmo_capsule();
			NormMesh mesh = create_mesh(vertices, ruby_material);
			drawables.push_back(Model<NormMesh>{mesh, model});
			normalvector_drawables.push_back(Model<NormMesh>{mesh, model});
		}
		else {
			throw std::runtime_error("impossible object is trying to be created");
		}

	
		if (draw_type == DrawType::Phong) {
			phong_drawer.draw(camera.view(),
							  projection,
							  totaltime,
							  directionallight,
							  pointlight,
							  drawables);
		}
		else if (draw_type == DrawType::Wireframe) {
			wireframe_drawer.draw(camera.view(),
								  projection,
								  totaltime,
								  wireframe_red,
								  drawables);
		}
		else if (draw_type == DrawType::PhongAndWireframe) {
			phong_drawer.draw(camera.view(),
							  projection,
							  totaltime,
							  directionallight,
							  pointlight,
							  drawables);
			wireframe_drawer.draw(camera.view(),
								  projection,
								  totaltime,
								  wireframe_red,
								  drawables);
		}
		else if (draw_type == DrawType::PhongNormalWireframe) {
			wireframe_drawer.draw(camera.view(),
								  projection,
								  totaltime,
								  wireframe_red,
								  drawables);
			wireframe_drawer.draw(camera.view(),
								  projection,
								  totaltime,
								  wireframe_blue,
								  normalvector_drawables);
		}
		else if (draw_type == DrawType::NormalColor) {
			normalcolor_drawer.draw(camera.view(),
									projection,
									totaltime,
									drawables);
		}

		SDL_GL_SwapWindow(window);
		GL_THROW_ON_ERROR();
		deltaclock.tick();
	}

	SDL_Quit();
	return 0;
}
