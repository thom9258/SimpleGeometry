#pragma once

#define SIMPLE_GEOMETRY_IMPLEMENTATION
#include "../../simple_geometry.h"

#include "utils/Random.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

struct vertex_posnorm {
	glm::vec3 pos;
	glm::vec3 norm;
	
};

std::string stringify(vertex_posnorm& v)
{
	std::stringstream ss;
	ss << "[ " << v.pos.x << " " << v.pos.y << " " << v.pos.z << "  |  "
	   <<  v.norm.x << " " << v.norm.y << " " << v.norm.z << " ]";
	return ss.str();
}


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
	float shininess;
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
	
struct DirectionalLight {
	DirectionalLight(const glm::vec3 direction, const sg_material mat)
		: direction(direction)
		, material(mat)
	{}

	glm::vec3 direction;
	Material material;
};

[[nodiscard]]
NormMesh cube(const sg_material mat)
{
	sg_status status;
	size_t vertices_length{0};
	
	sg_cube_info cube_info{};
	cube_info.width = 0.5f;
	cube_info.height = 0.5f;
	cube_info.depth = 0.5f;
	
	status = sg_cube_vertices(&cube_info, &vertices_length, nullptr, nullptr, nullptr);
	if (status != SG_OK_RETURNED_LENGTH)
		throw std::runtime_error("Could not get positions size");

	std::vector<sg_position> positions(vertices_length);
	std::vector<sg_normal> normals(vertices_length);
	status = sg_cube_vertices(&cube_info,
							  &vertices_length,
							  positions.data(),
							  normals.data(), 
							  nullptr);
	if (status != SG_OK_RETURNED_BUFFER)
		throw std::runtime_error("Could not get vertices");

	std::vector<vertex_posnorm> vertices_posnorm(positions.size());

	sg_strided_blockcopy_source_info positions_copy;
	positions_copy.ptr = positions.data();
	positions_copy.block_size = sizeof(positions[0]);
	positions_copy.stride = sizeof(positions[0]);
	positions_copy.block_count = positions.size();
	
	status = sg_strided_blockcopy(&positions_copy,
								  sizeof(vertices_posnorm[0]),
								  vertices_posnorm.data());

	if (status != SG_OK_COPIED_TO_DST)
		throw std::runtime_error("Could not copy positions to vertices");

	sg_strided_blockcopy_source_info normals_copy;
	normals_copy.ptr = normals.data();
	normals_copy.block_size = sizeof(normals[0]);
	normals_copy.stride = sizeof(normals[0]);
	normals_copy.block_count = normals.size();

	status = sg_strided_blockcopy(&normals_copy,
								  sizeof(vertices_posnorm[0]),
								  reinterpret_cast<char*>(vertices_posnorm.data()) 
								  + sizeof(vertices_posnorm[0].pos));

	if (status != SG_OK_COPIED_TO_DST)
		throw std::runtime_error("Could not copy positions to vertices");

	NormMesh mesh;
	mesh.material = mat;
	mesh.length = vertices_posnorm.size();

	// Setup VAO
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);
	GL_THROW_ON_ERROR();
	
	// Setup VBO
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);  
	GL_THROW_ON_ERROR();
	const size_t buffersize = vertices_posnorm.size() * sizeof(vertex_posnorm);
	glBufferData(GL_ARRAY_BUFFER, buffersize, vertices_posnorm.data(), GL_STATIC_DRAW);
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
	sg_indexed_sphere_info sphere_info{};
	sphere_info.radius = 0.5f;
	sphere_info.slices = 16;
	sphere_info.stacks = 16;

	sg_status status;
	size_t vertices_length{0};
	status = sg_indexed_sphere_vertices(&sphere_info,
										&vertices_length,
										nullptr,
										nullptr,
										nullptr);

	if (status != SG_OK_RETURNED_LENGTH)
		throw std::runtime_error("Could not get positions size");

	std::vector<sg_position> positions(vertices_length);
	std::vector<sg_normal> normals(vertices_length);

	status = sg_indexed_sphere_vertices(&sphere_info,
										&vertices_length,
										positions.data(),
										normals.data(), 
										nullptr);

	if (status != SG_OK_RETURNED_BUFFER)
		throw std::runtime_error("Could not get vertices");

	std::vector<vertex_posnorm> vertices_posnorm(positions.size());
	
	sg_strided_blockcopy_source_info positions_copy;
	positions_copy.ptr = positions.data();
	positions_copy.block_size = sizeof(positions[0]);
	positions_copy.stride = sizeof(positions[0]);
	positions_copy.block_count = positions.size();
	
	status = sg_strided_blockcopy(&positions_copy,
								  sizeof(vertices_posnorm[0]),
								  vertices_posnorm.data());

	if (status != SG_OK_COPIED_TO_DST)
		throw std::runtime_error("Could not copy positions to vertices");

	sg_strided_blockcopy_source_info normals_copy;
	normals_copy.ptr = normals.data();
	normals_copy.block_size = sizeof(normals[0]);
	normals_copy.stride = sizeof(normals[0]);
	normals_copy.block_count = normals.size();

	status = sg_strided_blockcopy(&normals_copy,
								  sizeof(vertices_posnorm[0]),
								  reinterpret_cast<char*>(vertices_posnorm.data()) 
								  + sizeof(vertices_posnorm[0].pos));

	if (status != SG_OK_COPIED_TO_DST)
		throw std::runtime_error("Could not copy positions to vertices");

	size_t indice_length;
	status = sg_indexed_sphere_indices(&sphere_info,
									   &indice_length,
									   nullptr);
	if (status != SG_OK_RETURNED_LENGTH)
		throw std::runtime_error("Could not get indices length");
	
	std::vector<SG_indice> indices(indice_length);
	status = sg_indexed_sphere_indices(&sphere_info,
									   &indice_length,
									   indices.data());

	if (status != SG_OK_RETURNED_BUFFER)
		throw std::runtime_error("Could not get indices");
	

	IndexedNormMesh mesh{};
	mesh.material = mat;
	mesh.indice_length = indices.size();

	// Setup VAO
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);
	GL_THROW_ON_ERROR();
	
	// Setup VBO
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);  
	GL_THROW_ON_ERROR();
	glBufferData(GL_ARRAY_BUFFER,
				 vertices_posnorm.size() * sizeof(vertex_posnorm),
				 vertices_posnorm.data(),
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
