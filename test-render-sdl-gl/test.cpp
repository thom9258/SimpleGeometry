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
const std::string shader_path = "../shaders/";


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

template<class... Ts> struct variant_switch : Ts... { using Ts::operator()...; };
template<class... Ts> variant_switch(Ts...) -> variant_switch<Ts...>;

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

class NormalDirectionDrawer
{
	std::shared_ptr<ValidShader> m_shader;

public:
	explicit NormalDirectionDrawer()
	{
		const auto vert = ShaderBuilder::file_slurp(shader_path + "normal.vert");
		if (!vert.has_value())
			throw std::runtime_error("could not load normal.vert");
		
		const auto frag = ShaderBuilder::file_slurp(shader_path + "normal.frag");
		if (!frag.has_value())
			throw std::runtime_error("could not load normal.frag");

		auto shader = ShaderBuilder::produce(vert.value(), frag.value());

		if (std::holds_alternative<InvalidShader>(shader))
			throw std::runtime_error(std::get<InvalidShader>(shader).what);
		m_shader = std::move(std::get<std::unique_ptr<ValidShader>>(shader));
	}
	
	auto get(const glm::mat4 view,
			 const glm::mat4 projection,
			 const float totaltime)
	{
		// we need to curry our shader into the returned functor, to do this
		// we create a local copy
		auto shader = m_shader;
		return variant_switch {
			[shader, view, projection, totaltime] (RandomRotated<NormMesh>& draw) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.rotator.final_model_matrix(totaltime));
				GL_THROW_ON_ERROR();
				glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
				GL_THROW_ON_ERROR();
			},
			[shader, view, projection, totaltime] (RandomRotated<IndexedNormMesh>& draw) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.rotator.final_model_matrix(totaltime));
				GL_THROW_ON_ERROR();
				glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
				GL_THROW_ON_ERROR();
			},
			[shader, view, projection] (Model<NormMesh>& draw) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.model);
				GL_THROW_ON_ERROR();
				glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
				GL_THROW_ON_ERROR();

			},
			[shader, view, projection] (Model<IndexedNormMesh>& draw) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.model);
				GL_THROW_ON_ERROR();
				glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
				GL_THROW_ON_ERROR();
			},
			[] (auto& draw) {
				throw std::runtime_error(std::string("could not handle drawable type [")
										 + typeid(draw).name() + "]");
			}};
	}
};

class WireframeDrawer
{
	std::shared_ptr<ValidShader> m_shader;

public:
	explicit WireframeDrawer()
	{
		const auto vert = ShaderBuilder::file_slurp(shader_path + "solidcolor.vert");
		if (!vert.has_value())
			throw std::runtime_error("could not load solidcolor.vert");
		
		const auto frag = ShaderBuilder::file_slurp(shader_path + "solidcolor.frag");
		if (!frag.has_value())
			throw std::runtime_error("could not load solidcolor.frag");

		auto shader = ShaderBuilder::produce(vert.value(), frag.value());

		if (std::holds_alternative<InvalidShader>(shader))
			throw std::runtime_error(std::get<InvalidShader>(shader).what);
		m_shader = std::move(std::get<std::unique_ptr<ValidShader>>(shader));
	}
	
	auto get(const glm::mat4 view,
			 const glm::mat4 projection,
			 const float totaltime,
			 const glm::vec3 color)
	{
		// we need to curry our shader into the returned functor, to do this
		// we create a local copy
		auto shader = m_shader;
		return variant_switch {
			[shader, view, projection, totaltime, color] (RandomRotated<NormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.rotator.final_model_matrix(totaltime));
				shader->set_vec3("color", color);
				GL_THROW_ON_ERROR();
				glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
				GL_THROW_ON_ERROR();
			},
			[shader, view, projection, totaltime, color] (RandomRotated<IndexedNormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.rotator.final_model_matrix(totaltime));
				shader->set_vec3("color", color);
				GL_THROW_ON_ERROR();
				glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
				GL_THROW_ON_ERROR();
			},
			[shader, view, projection, color] (Model<NormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.model);
				shader->set_vec3("color", color);
				GL_THROW_ON_ERROR();
				glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
				GL_THROW_ON_ERROR();
			},
			[shader, view, projection, color](Model<IndexedNormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.model);
				shader->set_vec3("color", color);
				GL_THROW_ON_ERROR();
				glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
				GL_THROW_ON_ERROR();
			},
			[] (auto& draw) 
			{
				throw std::runtime_error(std::string("could not handle drawable type [")
										 + typeid(draw).name() + "]");
			}};
	}
};

class PhongDrawer
{
	std::shared_ptr<ValidShader> m_shader;

public:
	explicit PhongDrawer()
	{
		const auto vert = ShaderBuilder::file_slurp(shader_path + "phong.vert");
		if (!vert.has_value())
			throw std::runtime_error("could not load phong vert");
		
		const auto frag = ShaderBuilder::file_slurp(shader_path + "phong.frag");
		if (!frag.has_value())
			throw std::runtime_error("could not load phong frag");
		
		auto shader = ShaderBuilder::produce(vert.value(), frag.value());

		if (std::holds_alternative<InvalidShader>(shader))
			throw std::runtime_error(std::get<InvalidShader>(shader).what);
		m_shader = std::move(std::get<std::unique_ptr<ValidShader>>(shader));
	}

	auto get(const glm::mat4 view,
			 const glm::mat4 projection,
			 const float totaltime,
			 const DirectionalLight directionallight,
			 const std::vector<PointLight> pointlights)
	{
		// we need to curry our shader into the returned functor, to do this
		// we create a local copy
		auto shader = m_shader;
		return variant_switch {
			[shader, view, projection, totaltime, directionallight, pointlights] 
			(RandomRotated<NormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.rotator.final_model_matrix(totaltime));
				
				shader->set_vec3("material.ambient", draw.mesh.material.ambient);
				shader->set_vec3("material.diffuse", draw.mesh.material.diffuse);
				shader->set_vec3("material.specular", draw.mesh.material.specular);
				shader->set_float("material.shininess", draw.mesh.material.shininess * 128);
				
				shader->set_vec3("pointlight.position", pointlights[0].position);
				shader->set_vec3("pointlight.ambient",  pointlights[0].material.ambient);
				shader->set_vec3("pointlight.diffuse",  pointlights[0].material.diffuse);
				shader->set_vec3("pointlight.specular", pointlights[0].material.specular);

				shader->set_vec3("directionallight.direction", directionallight.direction);
				shader->set_vec3("directionallight.ambient",   directionallight.material.ambient);
				shader->set_vec3("directionallight.diffuse",   directionallight.material.diffuse);
				shader->set_vec3("directionallight.specular",  directionallight.material.specular);

				GL_THROW_ON_ERROR();
				glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
				GL_THROW_ON_ERROR();
			},
			[shader, view, projection, totaltime, directionallight, pointlights] 
			(RandomRotated<IndexedNormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.rotator.final_model_matrix(totaltime));
				
				shader->set_vec3("material.ambient", draw.mesh.material.ambient);
				shader->set_vec3("material.diffuse", draw.mesh.material.diffuse);
				shader->set_vec3("material.specular", draw.mesh.material.specular);
				shader->set_float("material.shininess", draw.mesh.material.shininess * 128);
				
				shader->set_vec3("pointlight.position", pointlights[0].position);
				shader->set_vec3("pointlight.ambient",  pointlights[0].material.ambient);
				shader->set_vec3("pointlight.diffuse",  pointlights[0].material.diffuse);
				shader->set_vec3("pointlight.specular", pointlights[0].material.specular);

				shader->set_vec3("directionallight.direction", directionallight.direction);
				shader->set_vec3("directionallight.ambient",   directionallight.material.ambient);
				shader->set_vec3("directionallight.diffuse",   directionallight.material.diffuse);
				shader->set_vec3("directionallight.specular",  directionallight.material.specular);

				GL_THROW_ON_ERROR();
				glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
				GL_THROW_ON_ERROR();
			},
			[shader, view, projection, directionallight, pointlights] 
			(Model<NormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.model);
				
				shader->set_vec3("material.ambient", draw.mesh.material.ambient);
				shader->set_vec3("material.diffuse", draw.mesh.material.diffuse);
				shader->set_vec3("material.specular", draw.mesh.material.specular);
				shader->set_float("material.shininess", draw.mesh.material.shininess * 128);
				
				shader->set_vec3("pointlight.position", pointlights[0].position);
				shader->set_vec3("pointlight.ambient",  pointlights[0].material.ambient);
				shader->set_vec3("pointlight.diffuse",  pointlights[0].material.diffuse);
				shader->set_vec3("pointlight.specular", pointlights[0].material.specular);

				shader->set_vec3("directionallight.direction", directionallight.direction);
				shader->set_vec3("directionallight.ambient",   directionallight.material.ambient);
				shader->set_vec3("directionallight.diffuse",   directionallight.material.diffuse);
				shader->set_vec3("directionallight.specular",  directionallight.material.specular);

				GL_THROW_ON_ERROR();
				glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
				GL_THROW_ON_ERROR();
			},
			[shader, view, projection, directionallight, pointlights] 
			(Model<IndexedNormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->activate();
				glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
				shader->set_mat4("view", view);
				shader->set_mat4("proj", projection);
				shader->set_mat4("model", draw.model);
				
				shader->set_vec3("material.ambient", draw.mesh.material.ambient);
				shader->set_vec3("material.diffuse", draw.mesh.material.diffuse);
				shader->set_vec3("material.specular", draw.mesh.material.specular);
				shader->set_float("material.shininess", draw.mesh.material.shininess * 128);
				
				shader->set_vec3("pointlight.position", pointlights[0].position);
				shader->set_vec3("pointlight.ambient",  pointlights[0].material.ambient);
				shader->set_vec3("pointlight.diffuse",  pointlights[0].material.diffuse);
				shader->set_vec3("pointlight.specular", pointlights[0].material.specular);

				shader->set_vec3("directionallight.direction", directionallight.direction);
				shader->set_vec3("directionallight.ambient",   directionallight.material.ambient);
				shader->set_vec3("directionallight.diffuse",   directionallight.material.diffuse);
				shader->set_vec3("directionallight.specular",  directionallight.material.specular);

				GL_THROW_ON_ERROR();
				glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
				GL_THROW_ON_ERROR();
			},
			[] (auto& draw) 
			{
				throw std::runtime_error(std::string("could not handle drawable type [")
										 + typeid(draw).name() + "]");
			}};
	}
};

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
	
	std::vector<Drawable> scattered_drawables = {
		RandomRotated<NormMesh>(cube(sg_material_gold()), RandomModelRotator{}),
		RandomRotated<NormMesh>(cube(sg_material_obsidian()), RandomModelRotator{}),
		RandomRotated<NormMesh>(cube(sg_material_default_flat_white()), RandomModelRotator{}),
		RandomRotated<NormMesh>(cube(sg_material_ruby()), RandomModelRotator{}),
		RandomRotated<NormMesh>(cube(sg_material_emerald()), RandomModelRotator{}),
		
		RandomRotated<IndexedNormMesh>(sphere(sg_material_gold()), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(sg_material_obsidian()), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(sg_material_default_flat_white()), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(sg_material_ruby()), RandomModelRotator{}),
		RandomRotated<IndexedNormMesh>(sphere(sg_material_emerald()), RandomModelRotator{}),
	};

	auto floor_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	floor_model = glm::scale(floor_model, glm::vec3(10.0f, 0.05f, 10.0f));
	Drawable floor = Model<NormMesh>(cube(sg_material_default_flat_white()), floor_model);

	auto hill_model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, -3.0f));
	hill_model = glm::scale(hill_model, glm::vec3(5.0f));
	Drawable hill = Model<IndexedNormMesh>(sphere(sg_material_emerald()), hill_model);
	
	DrawType draw_type = DrawType::Phong;
	bool exit = false;
	
	NormalDirectionDrawer normal_direction_drawer{};
	WireframeDrawer wireframe_drawer{};
	PhongDrawer phong_drawer{};

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
					draw_type = next_draw_type(draw_type);
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
		
	
		const auto draw_world = [&] (auto drawer) {
			std::visit(drawer, floor);
			std::visit(drawer, hill);
			
			for (auto& drawable: scattered_drawables)
				std::visit(drawer, drawable);
		};
		
		if (draw_type == DrawType::NormalDirection) {
			draw_world(normal_direction_drawer.get(camera->view(), projection, totaltime));
		}
		else if (draw_type == DrawType::Wireframe) {
			const auto red = glm::vec3(1.0f, 0.0f, 0.0f);
			draw_world(wireframe_drawer.get(camera->view(),
											projection,
											totaltime,
											red));
		}
		else {
			const std::vector<PointLight> pointlights{
				PointLight(glm::vec3(0.0f, 1.0f, 0.0f), sg_material_ruby()),
			};
			const auto directionallight = DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f),
														   sg_material_default_flat_white());

			draw_world(phong_drawer.get(camera->view(),
										projection,
										totaltime,
										directionallight,
										pointlights));
		}
		

		SDL_GL_SwapWindow(window);
		GL_THROW_ON_ERROR();

		deltaclock.tick();
	}
}
