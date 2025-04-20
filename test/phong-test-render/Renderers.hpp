#pragma once

#include "utils/Shader.hpp"
#include "GenerateGeometries.hpp"

const std::string shader_path = "../shaders/";

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
		auto shader = ShaderBuilder::slurp_produce(shader_path + "normal.vert",
												   shader_path + "normal.frag");

		if (std::holds_alternative<InvalidShader>(shader))
			throw std::runtime_error(std::get<InvalidShader>(shader).what);
		m_shader = std::move(std::get<std::unique_ptr<ValidShader>>(shader));
	}
	
	auto operator()(const glm::mat4 view,
					const glm::mat4 projection,
					const float totaltime)
	{
		// we need to curry our shader into the returned functor, to do this
		// we create a local copy
		auto shader = m_shader;
		return variant_switch {
			[shader, view, projection, totaltime] (RandomRotated<NormMesh>& draw) {
				glEnable(GL_CULL_FACE);  
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.rotator.final_model_matrix(totaltime));
					GL_THROW_ON_ERROR();
					glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
					GL_THROW_ON_ERROR();
				});
			},
			[shader, view, projection, totaltime] (RandomRotated<IndexedNormMesh>& draw) {
				glEnable(GL_CULL_FACE);  
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.rotator.final_model_matrix(totaltime));
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				});
			},
			[shader, view, projection] (Model<NormMesh>& draw) {
				glEnable(GL_CULL_FACE);  
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.model);
					GL_THROW_ON_ERROR();
					glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
					GL_THROW_ON_ERROR();
				});

			},
			[shader, view, projection] (Model<IndexedNormMesh>& draw) {
				glEnable(GL_CULL_FACE);  
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.model);
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				});
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
		auto shader = ShaderBuilder::slurp_produce(shader_path + "solidcolor.vert",
												   shader_path + "solidcolor.frag");

		if (std::holds_alternative<InvalidShader>(shader))
			throw std::runtime_error(std::get<InvalidShader>(shader).what);
		m_shader = std::move(std::get<std::unique_ptr<ValidShader>>(shader));
	}
	
	auto operator()(const glm::mat4 view,
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
				glDisable(GL_CULL_FACE);  
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.rotator.final_model_matrix(totaltime));
					activated.set_vec3("color", color);
					GL_THROW_ON_ERROR();
					glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
					GL_THROW_ON_ERROR();
				});
			},
			[shader, view, projection, totaltime, color] (RandomRotated<IndexedNormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);  
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.rotator.final_model_matrix(totaltime));
					activated.set_vec3("color", color);
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				});
			},
			[shader, view, projection, color] (Model<NormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);  
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.model);
					activated.set_vec3("color", color);
					GL_THROW_ON_ERROR();
					glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
					GL_THROW_ON_ERROR();
				});
			},
			[shader, view, projection, color](Model<IndexedNormMesh>& draw) 
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);  
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.model);
					activated.set_vec3("color", color);
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				});
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

		auto shader = ShaderBuilder::slurp_produce(shader_path + "phong.vert",
												   shader_path + "phong.frag");

		if (std::holds_alternative<InvalidShader>(shader))
			throw std::runtime_error(std::get<InvalidShader>(shader).what);
		m_shader = std::move(std::get<std::unique_ptr<ValidShader>>(shader));
	}

	auto operator()(const glm::mat4 view,
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
				glEnable(GL_CULL_FACE);  
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.rotator.final_model_matrix(totaltime));
					activated.set_vec3("material.ambient", draw.mesh.material.ambient);
					activated.set_vec3("material.diffuse", draw.mesh.material.diffuse);
					activated.set_vec3("material.specular", draw.mesh.material.specular);
					activated.set_float("material.shininess", draw.mesh.material.shininess * 128);

					activated.set_vec3("pointlight.position", pointlights[0].position);
					activated.set_vec3("pointlight.ambient",  pointlights[0].material.ambient);
					activated.set_vec3("pointlight.diffuse",  pointlights[0].material.diffuse);
					activated.set_vec3("pointlight.specular", pointlights[0].material.specular);

					activated.set_vec3("directionallight.direction", directionallight.direction);
					activated.set_vec3("directionallight.ambient",
									   directionallight.material.ambient);
					activated.set_vec3("directionallight.diffuse",
									   directionallight.material.diffuse);
					activated.set_vec3("directionallight.specular",
									   directionallight.material.specular);
					GL_THROW_ON_ERROR();
					glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
					GL_THROW_ON_ERROR();

				});
			},
			[shader, view, projection, totaltime, directionallight, pointlights] 
			(RandomRotated<IndexedNormMesh>& draw) 
			{
				glEnable(GL_CULL_FACE);  
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);

				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.rotator.final_model_matrix(totaltime));
					activated.set_vec3("material.ambient", draw.mesh.material.ambient);
					activated.set_vec3("material.diffuse", draw.mesh.material.diffuse);
					activated.set_vec3("material.specular", draw.mesh.material.specular);
					activated.set_float("material.shininess", draw.mesh.material.shininess * 128);
					activated.set_vec3("pointlight.position", pointlights[0].position);
					activated.set_vec3("pointlight.ambient",  pointlights[0].material.ambient);
					activated.set_vec3("pointlight.diffuse",  pointlights[0].material.diffuse);
					activated.set_vec3("pointlight.specular", pointlights[0].material.specular);
					activated.set_vec3("directionallight.direction", directionallight.direction);
					activated.set_vec3("directionallight.ambient",
									   directionallight.material.ambient);
					activated.set_vec3("directionallight.diffuse",
									   directionallight.material.diffuse);
					activated.set_vec3("directionallight.specular",
									 directionallight.material.specular);
					
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				});
			},
			[shader, view, projection, directionallight, pointlights] 
			(Model<NormMesh>& draw) 
			{
				glEnable(GL_CULL_FACE);  
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.model);
					activated.set_vec3("material.ambient", draw.mesh.material.ambient);
					activated.set_vec3("material.diffuse", draw.mesh.material.diffuse);
					activated.set_vec3("material.specular", draw.mesh.material.specular);
					activated.set_float("material.shininess", draw.mesh.material.shininess * 128);
					activated.set_vec3("pointlight.position", pointlights[0].position);
					activated.set_vec3("pointlight.ambient",  pointlights[0].material.ambient);
					activated.set_vec3("pointlight.diffuse",  pointlights[0].material.diffuse);
					activated.set_vec3("pointlight.specular", pointlights[0].material.specular);
					activated.set_vec3("directionallight.direction", directionallight.direction);
					activated.set_vec3("directionallight.ambient",   directionallight.material.ambient);
					activated.set_vec3("directionallight.diffuse",
									   directionallight.material.diffuse);
					activated.set_vec3("directionallight.specular",
									   directionallight.material.specular);
					
					GL_THROW_ON_ERROR();
					glDrawArrays(GL_TRIANGLES, 0, draw.mesh.length);
					GL_THROW_ON_ERROR();
				});
			},
			[shader, view, projection, directionallight, pointlights] 
			(Model<IndexedNormMesh>& draw) 
			{
				glEnable(GL_CULL_FACE);  
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				GL_THROW_ON_ERROR();
				glBindVertexArray(draw.mesh.vao);
				shader->with_activated([&] (ActivatedShader& activated) 
				{
					glBindBuffer(GL_ARRAY_BUFFER, draw.mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw.mesh.ebo);
					activated.set_mat4("view", view);
					activated.set_mat4("proj", projection);
					activated.set_mat4("model", draw.model);
					activated.set_vec3("material.ambient", draw.mesh.material.ambient);
					activated.set_vec3("material.diffuse", draw.mesh.material.diffuse);
					activated.set_vec3("material.specular", draw.mesh.material.specular);
					activated.set_float("material.shininess", draw.mesh.material.shininess * 128);
					activated.set_vec3("pointlight.position", pointlights[0].position);
					activated.set_vec3("pointlight.ambient",  pointlights[0].material.ambient);
					activated.set_vec3("pointlight.diffuse",  pointlights[0].material.diffuse);
					activated.set_vec3("pointlight.specular", pointlights[0].material.specular);
					activated.set_vec3("directionallight.direction", directionallight.direction);
					activated.set_vec3("directionallight.ambient",
									   directionallight.material.ambient);
					activated.set_vec3("directionallight.diffuse",
									   directionallight.material.diffuse);
					activated.set_vec3("directionallight.specular",
									   directionallight.material.specular);
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, draw.mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				});
			},
			[] (auto& draw) 
			{
				throw std::runtime_error(std::string("could not handle drawable type [")
										 + typeid(draw).name() + "]");
			}};
	}
};
