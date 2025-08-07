#pragma once

#include "utils/Shader.hpp"
#include "Geometry.hpp"

const std::string shader_path = "../shaders/";

template<class... Ts> struct variant_switch : Ts... { using Ts::operator()...; };
template<class... Ts> variant_switch(Ts...) -> variant_switch<Ts...>;

template <typename Mesh>
struct Model {
	explicit Model(const Mesh mesh, const glm::mat4 model)
		: mesh(mesh), model(model) 
	{}
	Mesh mesh;
	glm::mat4 model;
};

using Drawable = std::variant<Model<NormMesh>,
							  Model<IndexedNormMesh>>;

class NormalDirectionDrawer
{
	ValidShader m_shader;

public:
	NormalDirectionDrawer()
		: m_shader{get_valid_shader_or_throw(ShaderBuilder::slurp_produce(shader_path + "normal.vert",
													 shader_path + "normal.frag"))}
	{}
	
	void draw(glm::mat4 view,
			  glm::mat4 projection,
			  float totaltime,
			  std::vector<Drawable>& drawables)
	{
		glEnable(GL_CULL_FACE);  
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		GL_THROW_ON_ERROR();

		m_shader.with_activated([&] (ActivatedShader& activated) 
		{
			activated.set_mat4("view", view);
			activated.set_mat4("proj", projection);

			for (Drawable& drawable: drawables) {
				if (auto p = std::get_if<Model<NormMesh>>(&drawable)) {
					glBindVertexArray(p->mesh.vao);
					glBindBuffer(GL_ARRAY_BUFFER, p->mesh.vbo);
					activated.set_mat4("model", p->model);
					glDrawArrays(GL_TRIANGLES, 0, p->mesh.length);
				}
				else if (auto p = std::get_if<Model<IndexedNormMesh>>(&drawable)) {
					glBindVertexArray(p->mesh.vao);
					glBindBuffer(GL_ARRAY_BUFFER, p->mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->mesh.ebo);
					activated.set_mat4("model", p->model);
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, p->mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				}
				else {
					throw std::runtime_error("could not handle drawable type");
				}

				GL_THROW_ON_ERROR();
			};
		});
	}
};

class WireframeDrawer
{
	ValidShader m_shader;

public:
	WireframeDrawer()
		: m_shader{get_valid_shader_or_throw(ShaderBuilder::slurp_produce(shader_path + "solidcolor.vert", shader_path + "solidcolor.frag"))}
	{}
	
	void draw(glm::mat4 view,
			  glm::mat4 projection,
			  float totaltime,
			  glm::vec3 color,
			  std::vector<Drawable>& drawables)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);  
		GL_THROW_ON_ERROR();

		m_shader.with_activated([&] (ActivatedShader& activated) 
		{
			activated.set_mat4("view", view);
			activated.set_mat4("proj", projection);
			activated.set_vec3("color", color);

			for (Drawable& drawable: drawables) {
				if (auto p = std::get_if<Model<NormMesh>>(&drawable)) {
					glBindVertexArray(p->mesh.vao);
					glBindBuffer(GL_ARRAY_BUFFER, p->mesh.vbo);
					activated.set_mat4("model", p->model);
					glDrawArrays(GL_TRIANGLES, 0, p->mesh.length);
				}
				else if (auto p = std::get_if<Model<IndexedNormMesh>>(&drawable)) {
					glBindVertexArray(p->mesh.vao);
					glBindBuffer(GL_ARRAY_BUFFER, p->mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->mesh.ebo);
					activated.set_mat4("model", p->model);
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, p->mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				}
				else {
					throw std::runtime_error("could not handle drawable type");
				}

				GL_THROW_ON_ERROR();
			};
		});
	}
};

class PhongDrawer
{
	ValidShader m_shader;

public:
	explicit PhongDrawer()
		: m_shader{get_valid_shader_or_throw(ShaderBuilder::slurp_produce(shader_path + "phong.vert", shader_path + "phong.frag"))}
	{}
	
	void draw(glm::mat4 view,
			  glm::mat4 projection,
			  float totaltime,
			  DirectionalLight directionallight,
			  PointLight pointlight,
			  std::vector<Drawable>& drawables)
	{
		m_shader.with_activated([&] (ActivatedShader& activated) 
		{
			glEnable(GL_CULL_FACE);  
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			GL_THROW_ON_ERROR();

			activated.set_mat4("view", view);
			activated.set_mat4("proj", projection);
			activated.set_vec3("pointlight.position", pointlight.position);
			activated.set_vec3("pointlight.ambient",  pointlight.material.ambient);
			activated.set_vec3("pointlight.diffuse",  pointlight.material.diffuse);
			activated.set_vec3("pointlight.specular", pointlight.material.specular);
			activated.set_vec3("directionallight.direction",
							   directionallight.direction);
			activated.set_vec3("directionallight.ambient",
							   directionallight.material.ambient);
			activated.set_vec3("directionallight.diffuse",
							   directionallight.material.diffuse);
			activated.set_vec3("directionallight.specular",
							   directionallight.material.specular);
			
			for (Drawable& drawable: drawables) {
				if (auto p = std::get_if<Model<NormMesh>>(&drawable)) {
					glBindVertexArray(p->mesh.vao);
					glBindBuffer(GL_ARRAY_BUFFER, p->mesh.vbo);
					activated.set_mat4("model", p->model);
					activated.set_vec3("material.ambient", p->mesh.material.ambient);
					activated.set_vec3("material.diffuse", p->mesh.material.diffuse);
					activated.set_vec3("material.specular", p->mesh.material.specular);
					activated.set_float("material.shininess", p->mesh.material.shininess);

					glDrawArrays(GL_TRIANGLES, 0, p->mesh.length);
				}
				else if (auto p = std::get_if<Model<IndexedNormMesh>>(&drawable)) {
					glBindVertexArray(p->mesh.vao);
					glBindBuffer(GL_ARRAY_BUFFER, p->mesh.vbo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->mesh.ebo);
					activated.set_mat4("model", p->model);
					activated.set_vec3("material.ambient", p->mesh.material.ambient);
					activated.set_vec3("material.diffuse", p->mesh.material.diffuse);
					activated.set_vec3("material.specular", p->mesh.material.specular);
					activated.set_float("material.shininess", p->mesh.material.shininess);
					GL_THROW_ON_ERROR();
					glDrawElements(GL_TRIANGLES, p->mesh.indice_length, GL_UNSIGNED_INT, 0);
					GL_THROW_ON_ERROR();
				}
				else {
					throw std::runtime_error("could not handle drawable type");
				}

				GL_THROW_ON_ERROR();
			};
		});
	}
};
