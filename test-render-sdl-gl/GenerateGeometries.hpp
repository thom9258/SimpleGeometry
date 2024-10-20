#pragma once

#define SIMPLE_GEOMETRY_IMPLEMENTATION
#include "../simple_geometry.h"

#include "utils/Random.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

struct vertex_posnorm {
	glm::vec3 pos;
	glm::vec3 norm;
	
	std::string stringify()
	{
		std::stringstream ss;
		ss << "[ " << pos.x << " " << pos.y << " " << pos.z << "  |  ";
		ss <<  norm.x << " " << norm.y << " " << norm.z << " ]";
		return ss.str();
	}
};

struct Material {
	Material()
		: Material(sg_material_default_flat_white())
	{}

	Material(const sg_material mat)
		: ambient(glm::vec3(mat.ambient.x, mat.ambient.y, mat.ambient.z))
		, diffuse(glm::vec3(mat.diffuse.x, mat.diffuse.y, mat.diffuse.z))
		, specular(glm::vec3(mat.specular.x, mat.specular.y, mat.specular.z))
		, shininess(mat.shininess)
	{}

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	SG_float shininess;
};

struct NormMesh {
	GLuint vao;
	GLuint vbo;
	size_t length; 
	Material material;
};

struct IndexedNormMesh {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	size_t indice_length; 
	Material material;
};

struct PointLight {
	PointLight(const glm::vec3 position, const sg_material mat)
		: position(position)
		, material(mat)
	{}

	glm::vec3 position;
	Material material;
};
	

[[nodiscard]]
NormMesh cube(const sg_material mat)
{
	NormMesh mesh;
	mesh.material = mat;

	sg_status status;
	SG_size vertices_length{0};
	status = sg_cube(0.5f, 0.5f, 0.5f, &vertices_length, nullptr, nullptr, nullptr);
	if (status != SG_OK_RETURNED_LEN)
		throw std::runtime_error("Could not get positions size");

	std::vector<sg_position> positions(vertices_length);
	std::vector<sg_normal> normals(vertices_length);
	status = sg_cube(0.5f, 0.5f, 0.5f,
					 &vertices_length,
					 positions.data(),
					 normals.data(), 
					 nullptr);
	if (status != SG_OK_RETURNED_BUFFER)
		throw std::runtime_error("Could not get vertices");

	std::vector<vertex_posnorm> vertices(positions.size());
	mesh.length = vertices.size();

	size_t i = 0;
	for (auto& vertex: vertices) {
		vertex.pos.x = positions[i].x;
		vertex.pos.y = positions[i].y;
		vertex.pos.z = positions[i].z;
		vertex.norm.x = normals[i].x;
		vertex.norm.y = normals[i].y;
		vertex.norm.z = normals[i].z;
		i++;
	}

	// Setup VAO
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);
	GL_THROW_ON_ERROR();
	
	// Setup VBO
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);  
	GL_THROW_ON_ERROR();
	const size_t buffersize = vertices.size() * sizeof(vertex_posnorm);
	glBufferData(GL_ARRAY_BUFFER, buffersize, vertices.data(), GL_STATIC_DRAW);
	GL_THROW_ON_ERROR();
	
	// Setuo VAO Layout
	glEnableVertexAttribArray(0);
	GL_THROW_ON_ERROR();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
						  sizeof(vertex_posnorm), 
						  (void*)offsetof(vertex_posnorm, pos));
	GL_THROW_ON_ERROR();
	
	glEnableVertexAttribArray(1);
	GL_THROW_ON_ERROR();
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
						  sizeof(vertex_posnorm),
						  (void*)offsetof(vertex_posnorm, norm));
	GL_THROW_ON_ERROR();
	
	// Unbind VAO
	glBindVertexArray(0);
	GL_THROW_ON_ERROR();
	return mesh;
}

[[nodiscard]]
IndexedNormMesh sphere(const sg_material mat)
{
	constexpr float radius = 0.5f;
	constexpr auto slices = 16;
	constexpr auto stacks = 16;

	IndexedNormMesh mesh{};
	mesh.material = mat;

	sg_status status;
	SG_size vertices_length{0};
	
	status = sg_indexed_sphere(radius, slices, stacks,
							   &vertices_length,
							   nullptr,
							   nullptr,
							   nullptr);

	if (status != SG_OK_RETURNED_LEN)
		throw std::runtime_error("Could not get positions size");

	std::vector<sg_position> positions(vertices_length);
	std::vector<sg_normal> normals(vertices_length);

	status = sg_indexed_sphere(radius, slices, stacks,
							   &vertices_length,
							   positions.data(),
							   normals.data(), 
							   nullptr);

	if (status != SG_OK_RETURNED_BUFFER)
		throw std::runtime_error("Could not get vertices");

	std::vector<vertex_posnorm> vertices(positions.size());
	size_t i = 0;
	for (auto& vertex: vertices) {
		vertex.pos.x = positions[i].x;
		vertex.pos.y = positions[i].y;
		vertex.pos.z = positions[i].z;
		vertex.norm.x = normals[i].x;
		vertex.norm.y = normals[i].y;
		vertex.norm.z = normals[i].z;
		i++;
	}
	
	status = sg_indexed_sphere_indices(slices, stacks,
									   &mesh.indice_length,
									   nullptr);
	if (status != SG_OK_RETURNED_LEN)
		throw std::runtime_error("Could not get indices length");
	
	std::vector<SG_indice> indices(mesh.indice_length);
	mesh.indice_length = indices.size();

	status = sg_indexed_sphere_indices(slices, stacks,
									   &mesh.indice_length,
									   indices.data());

	if (status != SG_OK_RETURNED_BUFFER)
		throw std::runtime_error("Could not get indices");

	// Setup VAO
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);
	GL_THROW_ON_ERROR();
	
	// Setup VBO
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);  
	GL_THROW_ON_ERROR();
	glBufferData(GL_ARRAY_BUFFER,
				 vertices.size() * sizeof(vertex_posnorm),
				 vertices.data(),
				 GL_STATIC_DRAW);
	GL_THROW_ON_ERROR();
	
	// Setup EBO
	glGenBuffers(1, &mesh.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);  
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 indices.size() * sizeof(SG_indice),
				 indices.data(),
				 GL_STATIC_DRAW);
	
	// Setup VAO Layout
	glEnableVertexAttribArray(0);
	GL_THROW_ON_ERROR();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
						  sizeof(vertex_posnorm), 
						  (void*)offsetof(vertex_posnorm, pos));
	GL_THROW_ON_ERROR();
	
	glEnableVertexAttribArray(1);
	GL_THROW_ON_ERROR();
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
						  sizeof(vertex_posnorm),
						  (void*)offsetof(vertex_posnorm, norm));
	GL_THROW_ON_ERROR();
	
	// Unbind VAO
	glBindVertexArray(0);
	GL_THROW_ON_ERROR();

	return mesh;
}
