
#define SIMPLE_GEOMETRY_IMPLEMENTATION
#include <SimpleGeometry/simple_geometry.h>

#include <cstddef>
#include <vector>
#include <format>
#include <algorithm>
#include <iostream>
#include <functional>

struct vertex {
	float pos[3];
	float norm[3];
	float uv[2];
	
};

void print_vertex(std::ostream* os, vertex& v)
{
	*os << std::format("[pos: {} {} {} norm: {} {} {} uv: {} {}]\n",
					   v.pos[0],
					   v.pos[1],
					   v.pos[2],
					   v.norm[0],
					   v.norm[1],
					   v.norm[2],
					   v.uv[0],
					   v.uv[1]);
}

int main() 
{
	sg_cube_info cube_info{
		.width = 0.5f,
		.height = 2.0f,
		.depth = 1.0f,
	};
	
	size_t vertices_length{0};
	sg_status status;
	status = sg_cube_vertices(&cube_info, &vertices_length, nullptr, nullptr, nullptr);
	if (status != SG_OK_RETURNED_LENGTH)
		throw std::runtime_error("Could not get positions size");

	std::vector<sg_position> positions(vertices_length);
	std::vector<sg_normal> normals(vertices_length);
	std::vector<sg_texcoord> uvs(vertices_length);
	status = sg_cube_vertices(&cube_info,
							  &vertices_length,
							  positions.data(),
							  normals.data(), 
							  uvs.data());
	if (status != SG_OK_RETURNED_BUFFER)
		throw std::runtime_error("Could not get vertices");

	std::vector<vertex> vertices(positions.size());

	sg_strided_blockcopy_source_info positions_copy{
		.ptr = positions.data(),
		.block_size = sizeof(positions[0]),
		.block_count = positions.size(),
		.stride = sizeof(positions[0]),
	};
	
	status = sg_strided_blockcopy(&positions_copy,
								  sizeof(vertices[0]),
								  vertices.data());

	if (status != SG_OK_COPIED_TO_DST)
		throw std::runtime_error("Could not copy positions to vertices");

	sg_strided_blockcopy_source_info normals_copy{
		.ptr = normals.data(),
		.block_size = sizeof(normals[0]),
		.block_count = normals.size(),
		.stride = sizeof(normals[0]),
	};

	status = sg_strided_blockcopy(&normals_copy,
								  sizeof(vertices[0]),
								  reinterpret_cast<char*>(vertices.data()) 
								  + offsetof(vertex, norm));

	if (status != SG_OK_COPIED_TO_DST)
		throw std::runtime_error("Could not copy normals to vertices");

	sg_strided_blockcopy_source_info uvs_copy {
		.ptr = uvs.data(),
		.block_size = sizeof(uvs[0]),
		.block_count = uvs.size(),
		.stride = sizeof(uvs[0]),
	};

	status = sg_strided_blockcopy(&uvs_copy,
								  sizeof(vertices[0]),
								  reinterpret_cast<char*>(vertices.data()) 
								  + offsetof(vertex, uv));

	if (status != SG_OK_COPIED_TO_DST)
		throw std::runtime_error("Could not copy uvs to vertices");

	std::ranges::for_each(vertices,
						  std::bind_front(print_vertex,
										  &std::cout));
}
