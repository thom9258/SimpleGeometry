/** 
* @file simple_geometry.h
* @brief Generate simple geometry.
*/

#ifndef _SIMPLE_GEOMETRY_H_
#define _SIMPLE_GEOMETRY_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#include <stddef.h>
#include <stdint.h>
	
#ifdef _WIN32
#  define SG_API_EXPORT __declspec(dllexport)
#else
#  define SG_API_EXPORT
#endif
	

#ifndef SG_GIZMO_CIRCLE_SUBDIVISIONS
#define SG_GIZMO_CIRCLE_SUBDIVISIONS 128
#endif
	
/** @addtogroup status
 *  @{
 */
	
enum sg_status {
	SG_OK_RETURNED_BUFFER,
	SG_OK_RETURNED_LENGTH,
	SG_OK_COPIED_TO_DST,

	SG_ERR_NULLPTR_INPUT,
	SG_ERR_ZEROSIZE_INPUT,
	SG_ERR_INFO_NOT_PROVIDED,
	SG_ERR_SUBDIVISIONS_MUST_BE_GREATER_THAN_1,
	SG_ERR_DSTLEN_NOT_PROVIDED,
	SG_ERR_SRCBLKSIZE_LESSTHAN_SRCSTRIDE,
	SG_ERR_SRCBLKSIZE_LESSTHAN_DSTSTRIDE,
	SG_ERR_VERTICES_NOT_DIVISIBLE_BY_3,

	SG_ERR_NOT_IMPLEMENTED_YET,
};


/**
 * @brief Check if status code is a success status.
 *
 * @return 'true' if status code signifies success,
 *         'false' otherwise.
 */
SG_API_EXPORT
bool
sg_success(const enum sg_status status); 
	
/**
 * @brief Convert a status code into a string literal.
 *
 * @param[in] status The status code to convert.
 *
 * @return A string literal identical to the definition of the status code.
 */
SG_API_EXPORT
const char*
sg_status_string(const enum sg_status status);
	

/**
 * @brief Convert a status code into a string literal.
 *
 * @param[in] src source pointer to copy bytes from.
 * @param[in] n bytecount to copy from 'src'.
 * @param[out] dst destination pointer to copy 'n' bytes to.
 *
 * @return status code describing the result of evaluation.
 */
SG_API_EXPORT
enum sg_status
sg_memcpy(
	const void* src,
	const size_t n,
	void* dst
);
	
struct sg_strided_blockcopy_source_info {
	void* ptr;          /// source pointer to first block.
	size_t block_size;  /// memory size of blocks.
	size_t block_count; /// amount of blocks.
	size_t stride;      /// stride of the individual blocks.
};

/**
 * @brief Convert a status code into a string literal.
 *
 * @param[in] src source pointer to copy bytes from.
 * @param[in] n bytecount to copy from 'src'.
 * @param[out] dst destination pointer to copy 'n' bytes to.
 *
 * @return status code describing the result of evaluation.
 */
SG_API_EXPORT
enum sg_status
sg_strided_blockcopy(
	struct sg_strided_blockcopy_source_info const* source,
	const size_t stride,
	void* dst
);
	
/** @}*/
	

/** @addtogroup vertex-types
 *  @{
 */

#ifndef	SG_indice
#  define SG_indice uint32_t
#endif
	
struct sg_position {
	float x;
	float y;
	float z;
};
	
struct sg_texcoord {
	float u;
	float v;
};

struct sg_normal {
	float x;
	float y;
	float z;
};

/** @}*/
	

/** @addtogroup geometry-generation
 *  @{
 */
struct sg_indexed_plane_info {
	float width;               /// width of the plane.
	float depth;               /// depth of the plane.
	size_t width_subdivisions; /// subdivisions along the width of the plane.
	size_t depth_subdivisions; /// subdivisions along the depth of the plane.
};
	
/**
 * @brief Generate vertices for a indexed & subdivided plane.
 *
 * @param[in]     plane     Information describing the geometry to generate.
 * @param[in out] length    The length of required vertex buffers to supply.
 * @param[out]    positions Vertex positions to generate.
 * @param[out]    normals   Vertex normals to generate.
 * @param[out]    texcoords Vertex texcoords to generate.
 *
 * @note To get the required length for the returned vertex buffers,
 *       provide a pointer to 'length' alongside all vertex buffer 
 *       pointers being NULL.
 *
 * @note The output vertex buffers MUST be provided with a length of at-least
 *       the returned 'length'.
 *
 * @note If any of the output vertex buffers are provided as NULL, the
 *       associated vertex data will not be generated and returned. 
 *
 * @return status code describing the result of evaluation.
 */
SG_API_EXPORT
enum sg_status
sg_indexed_plane_vertices(
	struct sg_indexed_plane_info const* plane,
	size_t* length,
	struct sg_position* positions,
	struct sg_normal* normals,
	struct sg_texcoord* texcoords
);
	
/**
 * @brief Generate indices for a indexed & subdivided plane.
 *
 * @param[in]     plane   Information describing the geometry to generate.
 * @param[in out] length  The length of required index buffer to supply.
 * @param[out]    indices Vertex indices to generate.
 *
 * @note To get the required length for the returned indice buffer,
 *       provide a pointer to 'length' alongside the indice buffer 
 *       pointer being NULL.
 *
 * @note The output index buffer MUST be provided with a length of at-least
 *       the returned 'length'.
 *
 * @return status code describing the result of evaluation.
 */
SG_API_EXPORT
enum sg_status
sg_indexed_plane_indices(
	struct sg_indexed_plane_info const* plane,
	size_t* length,
	SG_indice* indices
);

struct sg_cube_info {
	float width;  /// width of the cube.
	float height; /// height of the cube.
	float depth;  /// depth of the cube.
};
	

/**
 * @brief Generate vertices for a cube.
 *
 * @param[in]     cube      Information describing the geometry to generate.
 * @param[in out] length    The length of required vertex buffers to supply.
 * @param[out]    positions Vertex positions to generate.
 * @param[out]    normals   Vertex normals to generate.
 * @param[out]    texcoords Vertex texcoords to generate.
 *
 * @note To get the required length for the returned vertex buffers,
 *       provide a pointer to 'length' alongside all vertex buffer 
 *       pointers being NULL.
 *
 * @note The output vertex buffers MUST be provided with a length of at-least
 *       the returned 'length'.
 *
 * @note If any of the output vertex buffers are provided as NULL, the
 *       associated vertex data will not be generated and returned. 
 *
 * @return status code describing the result of evaluation.
 */
SG_API_EXPORT
enum sg_status
sg_cube_vertices(
	struct sg_cube_info* cube,
	size_t* length,
	struct sg_position* positions,
	struct sg_normal* normals,
	struct sg_texcoord* texcoords
);
	
	
struct sg_indexed_sphere_info {
	float radius;  /// Radius of the sphere.
	size_t slices; /// Subdivisions along the xy axis of the sphere.
	size_t stacks; /// Subdivisions along the z axis of the sphere.
};

/**
 * @brief Generate vertices for a indexed & subdivided uv sphere.
 *
 * @param[in]     sphere    Information describing the geometry to generate.
 * @param[in out] length    The length of required vertex buffers to supply.
 * @param[out]    positions Vertex positions to generate.
 * @param[out]    normals   Vertex normals to generate.
 * @param[out]    texcoords Vertex texcoords to generate.
 *
 * @note To get the required length for the returned vertex buffers,
 *       provide a pointer to 'length' alongside all vertex buffer 
 *       pointers being NULL.
 *
 * @note The output vertex buffers MUST be provided with a length of at-least
 *       the returned 'length'.
 *
 * @note If any of the output vertex buffers are provided as NULL, the
 *       associated vertex data will not be generated and returned. 
 *
 * @return status code describing the result of evaluation.
 */
SG_API_EXPORT
enum sg_status
sg_indexed_sphere_vertices(
	struct sg_indexed_sphere_info* sphere,
	size_t* length,
	struct sg_position* positions,
	struct sg_normal* normals,
	struct sg_texcoord* texcoords
);

/**
 * @brief Generate indices for a indexed & subdivided uv sphere.
 *
 * @param[in]     sphere  Information describing the geometry to generate.
 * @param[in out] length  The length of required index buffer to supply.
 * @param[out]    indices Vertex indices to generate.
 *
 * @note To get the required length for the returned indice buffer,
 *       provide a pointer to 'length' alongside the indice buffer 
 *       pointer being NULL.
 *
 * @note The output index buffer MUST be provided with a length of at-least
 *       the returned 'length'.
 *
 * @return status code describing the result of evaluation.
 */
SG_API_EXPORT
enum sg_status
sg_indexed_sphere_indices(
    struct sg_indexed_sphere_info* info,
    size_t* length,
    SG_indice* indices
);
	

struct sg_cylinder_info {
	float height;  /// Height of the cylinder.
	float top_radius;  /// radius of the top of the cylinder.
	float bottom_radius;  /// Radius of the bottom of the cylinder.
	size_t subdivisions; /// Subdivisions along the bottom of the cylinder.
};

/**
 * @brief Generate vertices for a cylinder.
 *
 * @param[in]     cylinder    Information describing the geometry to generate.
 * @param[in out] length    The length of required vertex buffers to supply.
 * @param[out]    positions Vertex positions to generate.
 * @param[out]    normals   Vertex normals to generate.
 * @param[out]    texcoords Vertex texcoords to generate.
 *
 * @note To get the required length for the returned vertex buffers,
 *       provide a pointer to 'length' alongside all vertex buffer 
 *       pointers being NULL.
 *
 * @note The output vertex buffers MUST be provided with a length of at-least
 *       the returned 'length'.
 *
 * @note If any of the output vertex buffers are provided as NULL, the
 *       associated vertex data will not be generated and returned. 
 *
 * @return status code describing the result of evaluation.
 */
SG_API_EXPORT
enum sg_status
sg_cylinder_vertices(
	struct sg_cylinder_info* cylinder,
	size_t* length,
	struct sg_position* positions,
	struct sg_normal* normals,
	struct sg_texcoord* texcoords
);
	
struct sg_gizmo_cone_info {
	float height;
	float radius;
};

/**
 * @brief Generate vertices for a gizmo cone.
 *
 * @param[in]     cone      Information describing the geometry to generate.
 * @param[out]    length    The length of required vertex buffers to supply.
 * @param[out]    positions Vertex positions to generate.
 *
 * @note To get the required length for the returned vertex buffer,
 *       provide a pointer to 'length' alongside all vertex buffer 
 *       pointers being NULL.
 *
 * @note The output vertex buffer MUST be provided with a length of at-least
 *       the returned 'length'.
 *
 * @return status code describing the result of evaluation.
 */
SG_API_EXPORT
enum sg_status
sg_gizmo_cone_vertices(
	struct sg_gizmo_cone_info* cone,
	size_t* length,
	struct sg_position* positions
);
	
struct sg_gizmo_sphere_info {
	float radius;
};

/**
 * @brief Generate vertices for a gizmo cone.
 *
 * @param[in]     cone      Information describing the geometry to generate.
 * @param[out]    length    The length of required vertex buffers to supply.
 * @param[out]    positions Vertex positions to generate.
 *
 * @note To get the required length for the returned vertex buffer,
 *       provide a pointer to 'length' alongside all vertex buffer 
 *       pointers being NULL.
 *
 * @note The output vertex buffer MUST be provided with a length of at-least
 *       the returned 'length'.
 *
 * @return status code describing the result of evaluation.
 */
SG_API_EXPORT
enum sg_status
sg_gizmo_sphere_vertices(
	struct sg_gizmo_sphere_info* sphere,
	size_t* length,
	struct sg_position* positions
);

/** @}*/
	

/** @addtogroup math
 *  @{
 */
#ifndef	SIMPLE_GEOMETRY_DONT_INCLUDE_MATH_H
#  include <math.h>
#  define SG_SQUARE_ROOT(V) sqrt(V)
#  define SG_COS(V) cos(V)
#  define SG_SIN(V) sin(V)
#  define SG_ATAN2(Y, X) atan2(Y, X)
#endif
	
#define SG_PI 3.1415926535897932384626433832795f
#define SG_2PI (2.0f * SG_PI)

#define SG_EXPAND_XYZ(T) (T.x), (T.y), (T.z)

struct sg_vec3f {
	float x; /// x direction of a 3D vector.
	float y; /// y direction of a 3D vector.
	float z; /// z direction of a 3D vector.
};
	
/**
 * @brief subtract two vec3's.
 * @param[in] a vector to be subtracted.
 * @param[in] b vector to use for subtraction.
 * @return a - b.
 */
SG_API_EXPORT
struct sg_vec3f
sg_vec3f_subtract(const struct sg_vec3f a,
				  const struct sg_vec3f b);

/**
 * @brief divide a vector3 with a divisor.
 * @param[in] a vector to be divided.
 * @param[in] f the divisor.
 * @return a / f.
 */
SG_API_EXPORT
struct sg_vec3f
sg_vec3f_dividef(const struct sg_vec3f a,
				 const float f);

/**
 * @brief Get the cross-product of two vector3's.
 * @param[in] a vector for cross-product calculation.
 * @param[in] b vector for cross-product calculation.
 * @return a x f.
 */
SG_API_EXPORT
struct sg_vec3f
sg_vec3f_cross(const struct sg_vec3f a,
			   const struct sg_vec3f b);

/**
 * @brief Get the length of a vector3.
 * @param[in] a vector to generate length from.
 * @return |a|.
 */
SG_API_EXPORT
float
sg_vec3f_length(const struct sg_vec3f a);

/**
 * @brief Normalize a vector3.
 * @param[in] a vector to normalize.
 * @return a / |a|.
 */
SG_API_EXPORT
struct sg_vec3f
sg_vec3f_normalize(const struct sg_vec3f a);

/**
 * @brief convert a position vertex into a vector3.
 * @param[in] pos the position vertex to convert.
 * @return the converted vector3.
 */
SG_API_EXPORT
struct sg_vec3f
sg_vec3f_from_position(const struct sg_position pos);

/**
 * @brief convert a vector3 into a normal vertex.
 * @param[in] v the vector3 to convert.
 * @return the converted normal vertex.
 */
SG_API_EXPORT
struct sg_normal
sg_normal_from_vec3f(const struct sg_vec3f v);
	
/** @}*/


/** @addtogroup materials
 *  @{
 */

struct sg_material {
	struct sg_vec3f ambient;  /// ambient color for material.
	struct sg_vec3f diffuse;  /// diffuse color for material.
	struct sg_vec3f specular; /// specular color for material.
	float shininess;          /// shininess color for material.
};

/**
 * @brief get the material properties of gold. * @return the material.
 */
SG_API_EXPORT
struct sg_material
sg_material_gold(); 

/**
 * @brief get the material properties of obsidian.
 * @return the material.
 */
SG_API_EXPORT
struct sg_material
sg_material_obsidian(); 
	
/**
 * @brief get the material properties of ruby.
 * @return the material.
 */
SG_API_EXPORT
struct sg_material
sg_material_ruby(); 

/**
 * @brief get the material properties of emerald.
 * @return the material.
 */
SG_API_EXPORT
struct sg_material
sg_material_emerald(); 

/**
 * @brief get the material properties of a standard flat white.
 * @return the material.
 */
SG_API_EXPORT
struct sg_material
sg_material_default_flat_white();
	
/** @}*/


/** ***********************************************************
 * Implementation
 */
#ifdef SIMPLE_GEOMETRY_IMPLEMENTATION

bool
sg_success(const enum sg_status status)
{
	switch (status) {
	case SG_OK_RETURNED_BUFFER:
	case SG_OK_RETURNED_LENGTH:
	case SG_OK_COPIED_TO_DST:
		return true;
	default:
		return false;
	};
}
	
const char*
sg_status_string(const enum sg_status status)
{
	switch (status) {
	case SG_OK_RETURNED_BUFFER:                      return "SG_OK_RETURNED_BUFFER"; 
	case SG_OK_RETURNED_LENGTH:                      return "SG_OK_RETURNED_LENGTH";
	case SG_OK_COPIED_TO_DST:                        return "SG_OK_COPIED_TO_DST";
	case SG_ERR_NULLPTR_INPUT:                       return "SG_ERR_NULLPTR_INPUT";
	case SG_ERR_ZEROSIZE_INPUT:                      return "SG_ERR_ZEROSIZE_INPUT";
	case SG_ERR_INFO_NOT_PROVIDED:                   return "SG_ERR_INFO_NOT_PROVIDED";
	case SG_ERR_SUBDIVISIONS_MUST_BE_GREATER_THAN_1: return "SG_ERR_SUBDIVISIONS_MUST_BE_GREATER_THAN_1";
	case SG_ERR_DSTLEN_NOT_PROVIDED:                 return "SG_ERR_DSTLEN_NOT_PROVIDED";
	case SG_ERR_SRCBLKSIZE_LESSTHAN_SRCSTRIDE:       return "SG_ERR_SRCBLKSIZE_LESSTHAN_SRCSTRIDE";
	case SG_ERR_SRCBLKSIZE_LESSTHAN_DSTSTRIDE:       return "SG_ERR_SRCBLKSIZE_LESSTHAN_DSTSTRIDE";
	case SG_ERR_NOT_IMPLEMENTED_YET:                 return "SG_ERR_NOT_IMPLEMENTED_YET";
	case SG_ERR_VERTICES_NOT_DIVISIBLE_BY_3:         return "SG_ERR_VERTICES_NOT_DIVISIBLE_BY_3";
	};
	return "SG_UNKNOWN_STATUS";
}

enum sg_status
sg_memcpy(
	const void* src,
	const size_t n,
	void* dst
)
{
	size_t i;
	if (src == NULL || dst == NULL)
		return SG_ERR_NULLPTR_INPUT;
	if (n < 1)
		return SG_ERR_ZEROSIZE_INPUT;

	for (i = 0; i < n; i++)
		((char*)dst)[i] = ((char*)src)[i];
	return SG_OK_COPIED_TO_DST;
}
	
enum sg_status
sg_strided_blockcopy(
	struct sg_strided_blockcopy_source_info const* source,
	const size_t stride,
	void* dst
)
{
	size_t i;
	char* currsrc;
	char* currdst;
	enum sg_status copystatus;

	if (source == NULL)
		return SG_ERR_INFO_NOT_PROVIDED;
	if (source->ptr == NULL || dst == NULL)
		return SG_ERR_NULLPTR_INPUT;
	if (stride < 1 || source->block_size < 1 || source->block_count < 1)
		return SG_ERR_ZEROSIZE_INPUT;
	if (stride < source->block_size)
		return SG_ERR_SRCBLKSIZE_LESSTHAN_DSTSTRIDE;
	
	for (i = 0; i < source->block_count; i++) {
		currsrc = ((char*)source->ptr) + (i * source->stride);
		currdst = ((char*)dst) + (i * stride);
		copystatus = sg_memcpy(currsrc, source->block_size, currdst);
		if (!sg_success(copystatus))
			return copystatus;
	}

	return SG_OK_COPIED_TO_DST;
}
	
struct sg_vec3f
sg_vec3f_subtract(const struct sg_vec3f a, const struct sg_vec3f b)
{
	struct sg_vec3f v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	v.z = a.z - b.z;
	return v;
}

struct sg_vec3f
sg_vec3f_dividef(const struct sg_vec3f a, const float f)
{
	struct sg_vec3f v;
	v.x = a.x / f;
	v.y = a.y / f;
	v.z = a.z / f;
	return v;
}

struct sg_vec3f
sg_vec3f_cross(const struct sg_vec3f a, const struct sg_vec3f b)
{
	struct sg_vec3f cross;
	cross.x = a.y*b.z - a.z*b.y;
	cross.y = a.z*b.x - a.x*b.z;
	cross.z = a.x*b.y - a.y*b.x;
	return cross;
}

float
sg_vec3f_length(const struct sg_vec3f a)
{
	return SG_SQUARE_ROOT(a.x*a.x + a.y*a.y + a.z*a.z);
}

struct sg_vec3f
sg_vec3f_normalize(const struct sg_vec3f a)
{
	float length;
	length = sg_vec3f_length(a);
	return sg_vec3f_dividef(a, length);
}

struct sg_vec3f
sg_vec3f_from_position(const struct sg_position pos)
{
	return (struct sg_vec3f) {pos.x, pos.y, pos.z};
}

struct sg_normal
sg_normal_from_vec3f(const struct sg_vec3f v)
{
	return (struct sg_normal) {v.x, v.y, v.z};
}

struct sg_normal
sg_face_normal(struct sg_position p1,
			   struct sg_position p2,
			   struct sg_position p3)
{
	struct sg_vec3f a;
	struct sg_vec3f b;
	struct sg_vec3f c;
	struct sg_vec3f ba;
	struct sg_vec3f ca;
	struct sg_vec3f cross;

	/* Calculate the face normal using the formula:
	 * n = normalize(cross(b-a, c-a)) 
	 */
	a = sg_vec3f_from_position(p1);
	b = sg_vec3f_from_position(p2);
	c = sg_vec3f_from_position(p3);

	ba = sg_vec3f_subtract(b, a); 
	ca = sg_vec3f_subtract(c, a); 
	cross = sg_vec3f_cross(ba, ca);
	return sg_normal_from_vec3f(sg_vec3f_normalize(cross));
}

enum sg_status
sg_calculate_flat_normals(
	const struct sg_position* vertices,
	const size_t vertices_length,
	struct sg_normal* normals
)
{
	struct sg_normal normal;
	size_t i;

	if (vertices == NULL || normals == NULL)
		return SG_ERR_NULLPTR_INPUT;
	if ((vertices_length % 3) != 0)
		return SG_ERR_VERTICES_NOT_DIVISIBLE_BY_3;

	for (i = 0; i < vertices_length; i += 3) {
		normal = sg_face_normal(vertices[i],
								vertices[i+1],
								vertices[i+2]);
		
		normals[i]   = normal;
		normals[i+1] = normal;
		normals[i+2] = normal;
	}

	return SG_OK_RETURNED_BUFFER;
}

enum sg_status
sg_indexed_plane_vertices(
	struct sg_indexed_plane_info const* plane,
	size_t* length,
	struct sg_position* positions,
	struct sg_normal* normals,
	struct sg_texcoord* texcoords
)
{
	if (plane == NULL)
		return SG_ERR_INFO_NOT_PROVIDED;
	if (length == NULL)
		return SG_ERR_DSTLEN_NOT_PROVIDED;
	if (plane->width_subdivisions < 1 || plane->depth_subdivisions < 1)
		return SG_ERR_SUBDIVISIONS_MUST_BE_GREATER_THAN_1;

	if (positions == NULL && normals == NULL && texcoords == NULL) {
		*length = plane->width_subdivisions * plane->depth_subdivisions;
		return SG_OK_RETURNED_LENGTH;
	}
	
	for (size_t w = 0; w < plane->width_subdivisions; w++) {
		for (size_t h = 0; h < plane->depth_subdivisions; h++) {
			//TODO: it might be we need depth_subdivisions here
			size_t index = h + (w * plane->width_subdivisions);
			float width_scale = plane->width / plane->width_subdivisions;
			float depth_scale = plane->depth / plane->depth_subdivisions;
			if (positions != NULL) {
				sg_position position {
					.x = width_scale * w,
					.y = depth_scale * h,
					.z = 0
				};
				positions[index] = position;
			}

			if (normals != NULL) {
				sg_normal normal {
					.x = 0,
					.y = 0,
					.z = 1 
				};
				normals[index] = normal;
			}

			if (texcoords != NULL) {
				sg_texcoord texcoord {
					.u = width_scale,
					.v = depth_scale,
				};
				texcoords[index] = texcoord;
			}
		}
	}

	return SG_OK_RETURNED_BUFFER;
}

enum sg_status
sg_indexed_plane_indices(
	struct sg_indexed_plane_info const* plane,
	size_t* length,
	SG_indice* indices
)
{
	(void)(plane);
	(void)(length);
	(void)(indices);
	return SG_ERR_NOT_IMPLEMENTED_YET;
}


#if 0
	const struct sg_normal _normals[36] = {
		{0.0f,  0.0f, -1.0f}, {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f, -1.0f}, 
		{0.0f,  0.0f, -1.0f}, {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f, -1.0f}, 

		{0.0f,  0.0f, 1.0f}, {0.0f,  0.0f, 1.0f}, {0.0f,  0.0f, 1.0f},
		{0.0f,  0.0f, 1.0f}, {0.0f,  0.0f, 1.0f}, {0.0f,  0.0f, 1.0f},

		{1.0f,  0.0f,  0.0f}, {1.0f,  0.0f,  0.0f}, {1.0f,  0.0f,  0.0f},
		{1.0f,  0.0f,  0.0f}, {1.0f,  0.0f,  0.0f}, {1.0f,  0.0f,  0.0f},

		{1.0f,  0.0f,  0.0f},{1.0f,  0.0f,  0.0f}, {1.0f,  0.0f,  0.0f},
		{1.0f,  0.0f,  0.0f},{1.0f,  0.0f,  0.0f}, {1.0f,  0.0f,  0.0f},

		{0.0f, -1.0f,  0.0f}, {0.0f, -1.0f,  0.0f}, {0.0f, -1.0f,  0.0f},
		{0.0f, -1.0f,  0.0f}, {0.0f, -1.0f,  0.0f}, {0.0f, -1.0f,  0.0f},

		{0.0f,  1.0f,  0.0f}, {0.0f,  1.0f,  0.0f}, {0.0f,  1.0f,  0.0f},
		{0.0f,  1.0f,  0.0f}, {0.0f,  1.0f,  0.0f}, {0.0f,  1.0f,  0.0f}
	};
#endif

enum sg_status
sg_cube_vertices(
	struct sg_cube_info* info,
	size_t* length,
	struct sg_position* positions,
	struct sg_normal* normals,
	struct sg_texcoord* texcoords
)
{
	if (info == NULL)
		return SG_ERR_INFO_NOT_PROVIDED;

	if (length == NULL)
		return SG_ERR_DSTLEN_NOT_PROVIDED;

	const struct sg_normal _normals[36] = {
		{-1.0f,  0.0f,  0.0f},
		{-1.0f,  0.0f,  0.0f},
		{-1.0f,  0.0f,  0.0f},
		{-1.0f,  0.0f,  0.0f},
		{-1.0f,  0.0f,  0.0f},
		{-1.0f,  0.0f,  0.0f},

		{0.0f,  0.0f, -1.0f},
		{0.0f,  0.0f, -1.0f},
		{0.0f,  0.0f, -1.0f},
		{0.0f,  0.0f, -1.0f},
		{0.0f,  0.0f, -1.0f},
		{0.0f,  0.0f, -1.0f},

		{0.0f, -1.0f,  0.0f},
		{0.0f, -1.0f,  0.0f},
		{0.0f, -1.0f,  0.0f},
		{0.0f, -1.0f,  0.0f},
		{0.0f, -1.0f,  0.0f},
		{0.0f, -1.0f,  0.0f},
		
		{0.0f,  1.0f,  0.0f},
		{0.0f,  1.0f,  0.0f},
		{0.0f,  1.0f,  0.0f},
		{0.0f,  1.0f,  0.0f},
		{0.0f,  1.0f,  0.0f},
		{0.0f,  1.0f,  0.0f},
		
		{1.0f,  0.0f,  0.0f},
		{1.0f,  0.0f,  0.0f},
		{1.0f,  0.0f,  0.0f},
		{1.0f,  0.0f,  0.0f},
		{1.0f,  0.0f,  0.0f},
		{1.0f,  0.0f,  0.0f},
		
		{0.0f,  0.0f,  1.0f},
		{0.0f,  0.0f,  1.0f},
		{0.0f,  0.0f,  1.0f},
		{0.0f,  0.0f,  1.0f},
		{0.0f,  0.0f,  1.0f},
		{0.0f,  0.0f,  1.0f}
	};

	//https://github.com/KhronosGroup/Vulkan-Tools/blob/main/cube/cube.cpp
	const struct sg_position _positions[36] = {
		{-info->width, -info->height, -info->depth},  // -X side
		{-info->width, -info->height,  info->depth},
		{-info->width,  info->height,  info->depth},
		{-info->width,  info->height,  info->depth},
		{-info->width,  info->height, -info->depth},
		{-info->width, -info->height, -info->depth},
		{-info->width, -info->height, -info->depth},  // -Z side
		{ info->width,  info->height, -info->depth},
		{ info->width, -info->height, -info->depth},
		{-info->width, -info->height, -info->depth},
		{-info->width,  info->height, -info->depth},
		{ info->width,  info->height, -info->depth},
		{-info->width, -info->height, -info->depth},  // -Y side
		{ info->width, -info->height, -info->depth},
		{ info->width, -info->height,  info->depth},
		{-info->width, -info->height, -info->depth},
		{ info->width, -info->height,  info->depth},
		{-info->width, -info->height,  info->depth},
		{-info->width,  info->height, -info->depth},  // +Y side
		{-info->width,  info->height,  info->depth},
		{ info->width,  info->height,  info->depth},
		{-info->width,  info->height, -info->depth},
		{ info->width,  info->height,  info->depth},
		{ info->width,  info->height, -info->depth},
		{ info->width,  info->height, -info->depth},  // +X side
		{ info->width,  info->height,  info->depth},
		{ info->width, -info->height,  info->depth},
		{ info->width, -info->height,  info->depth},
		{ info->width, -info->height, -info->depth},
		{ info->width,  info->height, -info->depth},
		{-info->width,  info->height,  info->depth},  // +Z side
		{-info->width, -info->height,  info->depth},
		{ info->width,  info->height,  info->depth},
		{-info->width, -info->height,  info->depth},
		{ info->width, -info->height,  info->depth},
		{ info->width,  info->height,  info->depth}
	};

	const struct sg_texcoord _texcoords[36] = {
		{0.0f, 1.0f},  // -X side
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{0.0f, 1.0f},

		{1.0f, 1.0f},  // -Z side
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 0.0f},

		{1.0f, 0.0f},  // -Y side
		{1.0f, 1.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{0.0f, 0.0f},

		{1.0f, 0.0f},  // +Y side
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},

		{1.0f, 0.0f},  // +X side
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},

		{0.0f, 0.0f},  // +Z side
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
	};
	

	size_t i;
	if (positions == NULL && normals == NULL && texcoords == NULL) {
		*length = 36;
		return SG_OK_RETURNED_LENGTH;
	}
	
	if (normals != NULL)
		for (i = 0; i < 36; ++i)
			normals[i] = _normals[i];

	if (positions != NULL)
		for (i = 0; i < 36; ++i)
			positions[i] = _positions[i];

	if (texcoords != NULL)
		for (i = 0; i < 36; ++i)
			texcoords[i] = _texcoords[i];
	
	return SG_OK_RETURNED_BUFFER;
}



enum sg_status
sg_indexed_sphere_vertices(
	struct sg_indexed_sphere_info* info,
	size_t* length,
	struct sg_position* positions,
	struct sg_normal* normals,
	struct sg_texcoord* texcoords
)
/**
 * Original Reference:
 * https://www.3dgep.com/texturing-and-lighting-with-opengl-and-glsl/#Creating_a_Sphere
 */
{
	if (info == NULL)
		return SG_ERR_INFO_NOT_PROVIDED;

	if (length == NULL)
		return SG_ERR_DSTLEN_NOT_PROVIDED;

	if (positions == NULL && normals == NULL && texcoords == NULL) {
		*length = (info->slices+1)*(info->stacks+1);
		return SG_OK_RETURNED_LENGTH;
	}

	size_t n = 0;
	for (size_t i = 0; i <= info->stacks; ++i) {
		sg_texcoord texcoord{
			.u = 0,
			.v = 0
		};

		texcoord.v = i / (float)info->stacks;
		float phi = texcoord.v * SG_PI;

		for (size_t j = 0; j <= info->slices; ++j) {
			texcoord.u = j / (float)info->slices;
			float theta = texcoord.u * SG_2PI;
			sg_normal normal{
				.x = SG_COS(theta) * SG_SIN(phi),
				.y = SG_COS(phi),
				.z = SG_SIN(theta) * SG_SIN(phi)
			};

			if (normals != NULL)
				normals[n] = normal;

			if (positions != NULL) {
				sg_position position{
					.x = normal.x * info->radius,
					.y = normal.y * info->radius,
					.z = normal.z * info->radius
				};
				positions[n] = position;
			}
			
			if (texcoords != NULL)
				texcoords[n] = texcoord;
			n++;
		}
	}

	return SG_OK_RETURNED_BUFFER;
}

enum sg_status
sg_indexed_sphere_indices(
	struct sg_indexed_sphere_info* info,
	size_t* length,
	SG_indice* indices
)
{
	if (info == NULL)
		return SG_ERR_INFO_NOT_PROVIDED;

	if (length == NULL)
		return SG_ERR_DSTLEN_NOT_PROVIDED;
	
	if (indices == NULL) {
		*length = (info->slices * info->stacks + info->slices) * 6;
		return SG_OK_RETURNED_LENGTH;
	}
	
	size_t n = 0;
	for (size_t i = 0; i < info->slices * info->stacks + info->slices; ++i) {
		indices[n++] = i;
		indices[n++] = i + info->slices + 1;
		indices[n++] = i + info->slices;

		indices[n++] = i + info->slices + 1;
		indices[n++] = i;
		indices[n++] = i + 1;
	}

	return SG_OK_RETURNED_BUFFER;
}

enum sg_status
sg_cylinder_vertices(
	struct sg_cylinder_info* cylinder,
	size_t* length,
	struct sg_position* positions,
	struct sg_normal* normals,
	struct sg_texcoord* texcoords
)
{
	if (cylinder == NULL)
		return SG_ERR_INFO_NOT_PROVIDED;

	if (length == NULL)
		return SG_ERR_DSTLEN_NOT_PROVIDED;
	
	if (positions == NULL && normals == NULL && texcoords == NULL) {
		*length = cylinder->subdivisions * 12;
		return SG_OK_RETURNED_LENGTH;
	}
	
	const float cylinder_top_radius = cylinder->top_radius;
	const float cylinder_bottom_radius = cylinder->bottom_radius;
	const float cylinder_height = cylinder->height;
	const size_t cylinder_subdivisions = cylinder->subdivisions;
	
	const float sector_step = 2 * SG_PI / cylinder_subdivisions;
	const struct sg_normal bottom_cap_normal{ 0.0f, -1.0f, 0.0f };
	const struct sg_normal top_cap_normal{ 0.0f, 1.0f, 0.0f };
	
	const float z_angle = SG_ATAN2(cylinder_bottom_radius - cylinder_top_radius,
								   cylinder_height);
	size_t vertex_count = 0;
	// Generate bottom cap
	for (size_t i = 0; i < cylinder_subdivisions; i++) {
		const float sector_angle = i * sector_step;
		const float next_sector_angle = (i+1) * sector_step;

		if (positions) {
			positions[vertex_count + 0] = (struct sg_position){ 0, -(cylinder_height / 2), 0 };
			positions[vertex_count + 1] = (struct sg_position){
				.x = SG_COS(sector_angle) * cylinder_bottom_radius,
				.y = -(cylinder_height / 2),
				.z = SG_SIN(sector_angle)* cylinder_bottom_radius
			};
			positions[vertex_count + 2] = (struct sg_position){
				.x = SG_COS(next_sector_angle) * cylinder_bottom_radius,
				.y = -(cylinder_height / 2),
				.z = SG_SIN(next_sector_angle) * cylinder_bottom_radius
			};
		}

		if (normals) {
			normals[vertex_count + 0] = bottom_cap_normal;
			normals[vertex_count + 1] = bottom_cap_normal;
			normals[vertex_count + 2] = bottom_cap_normal;
		}

		vertex_count += 3;
	}
	
	// Generate top cap
	for (size_t i = 0; i < cylinder_subdivisions; i++) {
		const float sector_angle = i * sector_step;
		const float next_sector_angle = (i+1) * sector_step;

		if (positions) {
			positions[vertex_count + 0] = (struct sg_position){ 0, cylinder_height / 2, 0 };
			positions[vertex_count + 1] = (struct sg_position){
				.x = SG_COS(next_sector_angle) * cylinder_top_radius,
				.y = cylinder_height / 2,
				.z = SG_SIN(next_sector_angle) * cylinder_top_radius
			};
			positions[vertex_count + 2] = (struct sg_position){
				.x = SG_COS(sector_angle) * cylinder_top_radius,
				.y = cylinder_height / 2,
				.z = SG_SIN(sector_angle)* cylinder_top_radius
			};
		}

		if (normals) {
			normals[vertex_count + 0] = top_cap_normal;
			normals[vertex_count + 1] = top_cap_normal;
			normals[vertex_count + 2] = top_cap_normal;
		}

		vertex_count += 3;
	}
	
	// Generate sides
	for (size_t i = 0; i < cylinder_subdivisions; i++) {
		const float sector_angle = i * sector_step;
		const float next_sector_angle = (i+1) * sector_step;

		if (positions) {
			struct sg_position bl{
				.x = SG_COS(sector_angle) * cylinder_bottom_radius,
				.y = -(cylinder_height / 2),
				.z = SG_SIN(sector_angle)* cylinder_bottom_radius
			};

			struct sg_position br{
				.x = SG_COS(next_sector_angle) * cylinder_bottom_radius,
				.y = -cylinder_height / 2,
				.z = SG_SIN(next_sector_angle)* cylinder_bottom_radius
			};
			struct sg_position tl{
				.x = SG_COS(sector_angle) * cylinder_top_radius,
				.y = cylinder_height / 2,
				.z = SG_SIN(sector_angle) * cylinder_top_radius
			};
			struct sg_position tr{
				.x = SG_COS(next_sector_angle) * cylinder_top_radius,
				.y = cylinder_height / 2,
				.z = SG_SIN(next_sector_angle) * cylinder_top_radius
			};
			

			positions[vertex_count + 0] = tl;
			positions[vertex_count + 1] = br;
			positions[vertex_count + 2] = bl;
			positions[vertex_count + 3] = tr;
			positions[vertex_count + 4] = br;
			positions[vertex_count + 5] = tl;
		}
		
		if (normals) {
			struct sg_normal left{
				.x = SG_COS(sector_angle),
				.y = z_angle,
				.z = SG_SIN(sector_angle)
			};
			struct sg_normal right{
				.x = SG_COS(next_sector_angle),
				.y = z_angle,
				.z = SG_SIN(next_sector_angle)
			};
			normals[vertex_count + 0] = left;
			normals[vertex_count + 1] = right;
			normals[vertex_count + 2] = left;
			normals[vertex_count + 3] = right;
			normals[vertex_count + 4] = right;
			normals[vertex_count + 5] = left;
		}

		vertex_count += 6;
	}
	
	return SG_OK_RETURNED_BUFFER;
}

enum sg_status
sg_gizmo_cone_vertices(
	struct sg_gizmo_cone_info* cone,
	size_t* length,
	struct sg_position* positions)
{
	if (!cone) return SG_ERR_INFO_NOT_PROVIDED;

	
	if (!positions) {
		if (!length) return SG_ERR_DSTLEN_NOT_PROVIDED;
		
		*length = 12 + SG_GIZMO_CIRCLE_SUBDIVISIONS * 3;
		return SG_OK_RETURNED_LENGTH;
	}
	
	const float cone_height = cone->height;
	const float cone_radius = cone->radius;
	const float sector_step = 2 * SG_PI / SG_GIZMO_CIRCLE_SUBDIVISIONS;
	
	size_t vertex_count = 0;
	
		// Generate top cap
	for (size_t i = 0; i < SG_GIZMO_CIRCLE_SUBDIVISIONS; i++) {
		const float sector_angle = i * sector_step;
		const float next_sector_angle = (i+1) * sector_step;

		const struct sg_position sector_position{
			.x = SG_COS(next_sector_angle) * cone_radius,
			.y = -(cone_height / 2),
			.z = SG_SIN(next_sector_angle) * cone_radius
		};
		const struct sg_position next_sector_position{
			.x = SG_COS(sector_angle) * cone_radius,
			.y = -(cone_height / 2),
			.z = SG_SIN(sector_angle)* cone_radius
		};
		
		positions[vertex_count + 0] = sector_position;
		positions[vertex_count + 1] = next_sector_position;
		positions[vertex_count + 2] = sector_position;
		vertex_count += 3;
	}
	
	const struct sg_position top{
		.x = 0,
		.y = cone_height / 2,
		.z = 0
	};
	const struct sg_position left{
		.x = cone_radius,
		.y = -(cone_height / 2),
		.z = 0
	};
	positions[vertex_count + 0] = top;
	positions[vertex_count + 1] = left;
	positions[vertex_count + 2] = top;
	vertex_count += 3;

	const struct sg_position right{
		.x = -cone_radius,
		.y = -(cone_height / 2),
		.z = 0
	};
	positions[vertex_count + 0] = top;
	positions[vertex_count + 1] = right;
	positions[vertex_count + 2] = top;
	vertex_count += 3;

	const struct sg_position front{
		.x = 0,
		.y = -(cone_height / 2),
		.z = cone_radius
	};
	positions[vertex_count + 0] = top;
	positions[vertex_count + 1] = front;
	positions[vertex_count + 2] = top;
	vertex_count += 3;

	const struct sg_position back{
		.x = 0,
		.y = -(cone_height / 2),
		.z = -cone_radius
	};
	positions[vertex_count + 0] = top;
	positions[vertex_count + 1] = back;
	positions[vertex_count + 2] = top;
	
	return SG_OK_RETURNED_BUFFER;
}

enum sg_status
sg_gizmo_sphere_vertices(
	struct sg_gizmo_sphere_info* sphere,
	size_t* length,
	struct sg_position* positions)
{
	if (!sphere) return SG_ERR_INFO_NOT_PROVIDED;

	
	if (!positions) {
		if (!length) return SG_ERR_DSTLEN_NOT_PROVIDED;
		
		*length = (SG_GIZMO_CIRCLE_SUBDIVISIONS * 3) * 3;
		return SG_OK_RETURNED_LENGTH;
	}
	
	const float sphere_radius = sphere->radius;
	const float sector_step = 2 * SG_PI / SG_GIZMO_CIRCLE_SUBDIVISIONS;
	
	size_t vertex_count = 0;
	
	// Generate x band
	for (size_t i = 0; i < SG_GIZMO_CIRCLE_SUBDIVISIONS; i++) {
		const float sector_angle = i * sector_step;
		const float next_sector_angle = (i+1) * sector_step;

		const struct sg_position sector_position{
			.x = 0,
			.y = SG_COS(next_sector_angle) * sphere_radius,
			.z = SG_SIN(next_sector_angle) * sphere_radius
		};
		const struct sg_position next_sector_position{
			.x = 0,
			.y = SG_COS(sector_angle) * sphere_radius,
			.z = SG_SIN(sector_angle)* sphere_radius
		};
		
		positions[vertex_count + 0] = sector_position;
		positions[vertex_count + 1] = next_sector_position;
		positions[vertex_count + 2] = sector_position;
		vertex_count += 3;
	}
	
	// Generate y band
	for (size_t i = 0; i < SG_GIZMO_CIRCLE_SUBDIVISIONS; i++) {
		const float sector_angle = i * sector_step;
		const float next_sector_angle = (i+1) * sector_step;

		const struct sg_position sector_position{
			.x = SG_COS(next_sector_angle) * sphere_radius,
			.y = 0,
			.z = SG_SIN(next_sector_angle) * sphere_radius
		};
		const struct sg_position next_sector_position{
			.x = SG_COS(sector_angle) * sphere_radius,
			.y = 0,
			.z = SG_SIN(sector_angle)* sphere_radius
		};
		
		positions[vertex_count + 0] = sector_position;
		positions[vertex_count + 1] = next_sector_position;
		positions[vertex_count + 2] = sector_position;
		vertex_count += 3;
	}
	
	// Generate z band
	for (size_t i = 0; i < SG_GIZMO_CIRCLE_SUBDIVISIONS; i++) {
		const float sector_angle = i * sector_step;
		const float next_sector_angle = (i+1) * sector_step;

		const struct sg_position sector_position{
			.x = SG_COS(next_sector_angle) * sphere_radius,
			.y = SG_SIN(next_sector_angle) * sphere_radius,
			.z = 0
		};
		const struct sg_position next_sector_position{
			.x = SG_COS(sector_angle) * sphere_radius,
			.y = SG_SIN(sector_angle)* sphere_radius,
			.z = 0
		};
		
		positions[vertex_count + 0] = sector_position;
		positions[vertex_count + 1] = next_sector_position;
		positions[vertex_count + 2] = sector_position;
		vertex_count += 3;
	}

	return SG_OK_RETURNED_BUFFER;
}



struct sg_material
sg_material_gold()
{
	struct sg_material mat;
	mat.ambient  = (struct sg_vec3f) {0.24725f, 0.1995f, 0.0745f};
	mat.diffuse  = (struct sg_vec3f) {0.75164f, 0.60648f, 0.22648f};
	mat.specular = (struct sg_vec3f) {0.628281f, 0.555802f, 0.366065f};
	mat.shininess = 0.4f;
	return mat;
}

struct sg_material
sg_material_obsidian()
{
	struct sg_material mat;
	mat.ambient  = (struct sg_vec3f) {0.05375f, 0.05f, 0.06625f};
	mat.diffuse  = (struct sg_vec3f) {0.18275f, 0.17f, 0.22525f};
	mat.specular = (struct sg_vec3f) {0.332741f, 0.328634f, 0.346435f};
	mat.shininess = 0.3f;
	return mat;
}

struct sg_material
sg_material_ruby()
{
	struct sg_material mat;
	mat.ambient  = (struct sg_vec3f) {0.1745f, 0.01175f, 0.01175f};
	mat.diffuse  = (struct sg_vec3f) {0.61424, 0.04136f, 0.04136};
	mat.specular = (struct sg_vec3f) {0.727811f, 0.626959f, 0.626959};
	mat.shininess = 0.6f;
	return mat;
}

struct sg_material
sg_material_emerald()
{
	struct sg_material mat;
	mat.ambient  = (struct sg_vec3f) {0.0215f, 0.1745f, 0.0215};
	mat.diffuse  = (struct sg_vec3f) {0.07568f, 0.61424f, 0.07568f};
	mat.specular = (struct sg_vec3f) {0.633f, 0.727811f, 0.633f};
	mat.shininess = 0.6f;
	return mat;
}
	
struct sg_material
sg_material_default_flat_white()
{
	struct sg_material mat;
	mat.ambient  = (struct sg_vec3f) {0.0f, 0.0f, 0.0f};
	mat.diffuse  = (struct sg_vec3f) {0.95f, 0.95f, 0.95f};
	mat.specular = (struct sg_vec3f) {0.7f, 0.7f, 0.7f};
	mat.shininess = 0.25f;
	return mat;
}
	
#endif // SIMPLE_GEOMETRY_IMPLEMENTATION
#ifdef __cplusplus
}
#endif
#endif //_SIMPLE_GEOMETRY_H_

