#ifndef _SIMPLE_GEOMETRY_H_
#define _SIMPLE_GEOMETRY_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#ifdef _WIN32
#  define SG_API_EXPORT __declspec(dllexport)
#else
#  define SG_API_EXPORT
#endif
	
#ifndef	SG_UNREFERENCED
#  define SG_UNREFERENCED(VAR) (void)(VAR)
#endif
	
/** ***********************************************************
 * Argument Annotations
 */
	
//TODO remove this annotation
#define SG_OUT
	
/**
* @brief Annotatate that the value is used.
*/
#define SG_uses

/**
* @brief Annotatate that the pointer must be provided and will accessed.
*/
#define SG_readsFrom

/**
* @brief Annotatate that the pointer must be provided and will be overwridden.
*/
#define SG_opt_writesTo
	
/**
* @brief Annotatate that the pointer to a buffer can be optionally provided,
*        but if it is provided, it must have the length specified by 'LENGTH'
*/
#define SG_opt_writesTo_withLength(LENGTH)
	
/** ***********************************************************
 * Types
 */
	
#ifndef	SG_nullptr
#  ifdef __cplusplus
#    define SG_nullptr nullptr
#  else
#    define SG_nullptr ((void*)0)
#  endif
#endif

#ifndef	SG_true
#  ifdef __cplusplus
#    define SG_true true
#  else
#    define SG_true 1
#  endif
#endif

#ifndef	SG_false
#  ifdef __cplusplus
#    define SG_false false
#  else
#    define SG_false 0 
#  endif
#endif

#ifndef	SG_size
#  ifdef __cplusplus
#    define SG_size size_t
#  else
#    define SG_size unsigned int
#  endif
#endif
	
#ifndef	SG_float
#  define SG_float float
#endif

#ifndef	SG_bool
#  ifdef __cplusplus
#    define SG_bool bool
#  else
#    define SG_bool char
#  endif
#endif
	
#ifndef	SG_indice
#  define SG_indice unsigned int
#endif
	
/** ***********************************************************
 * Utility
 */
	
#define	SG_ARRAY_MEMSIZE(ARR) sizeof(ARR)
#define	SG_ARRAY_LEN(ARR) (SG_ARRAY_MEMSIZE(ARR) / sizeof(ARR[0]))

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

SG_API_EXPORT
SG_bool
sg_success(SG_uses const enum sg_status status); 
	
SG_API_EXPORT
const char*
sg_status_string(SG_uses const enum sg_status status);
	
SG_API_EXPORT
enum sg_status
sg_memcpy(
	SG_readsFrom                   const void* src,
	SG_uses                        const SG_size n,
	SG_opt_writesTo_withLength(n)  void* dst
);

SG_API_EXPORT
enum sg_status
sg_strided_blockcopy(
	SG_readsFrom     struct sg_strided_blockcopy_source_info const* source,
	SG_uses          const SG_size stride,
	SG_opt_writesTo  void* dst
);

/** ***********************************************************
 * Geometry
 */
	
SG_API_EXPORT
enum sg_status
sg_calculate_flat_normals(
	SG_readsFrom                                 const struct sg_position* vertices,
	SG_uses                                      const SG_size vertices_length,
	SG_opt_writesTo_withLength(vertices_length)  struct sg_normal* normals
);

struct sg_position {
	SG_float x;
	SG_float y;
	SG_float z;
};
	
struct sg_texcoord {
	SG_float u;
	SG_float v;
};

struct sg_normal {
	SG_float x;
	SG_float y;
	SG_float z;
};
	
struct sg_indexed_plane_info {
	SG_float width;
	SG_float depth;
	SG_size width_subdivisions;
	SG_size depth_subdivisions;
};
	
struct sg_cube_info {
	SG_float width;
	SG_float height;
	SG_float depth;
};
	
struct sg_indexed_sphere_info {
	SG_float radius;
	SG_size slices;
	SG_size stacks;
};
	
SG_API_EXPORT
enum sg_status
sg_indexed_plane_vertices(
	SG_readsFrom                        struct sg_indexed_plane_info const* plane,
	SG_opt_writesTo                     SG_size* length,
	SG_opt_writesTo_withLength(length)  struct sg_position* positions,
	SG_opt_writesTo_withLength(length)  struct sg_normal* normals,
	SG_opt_writesTo_withLength(length)  struct sg_texcoord* texcoords
);
	
SG_API_EXPORT
enum sg_status
sg_indexed_plane_indices(
	SG_readsFrom                        struct sg_indexed_plane_info const* plane,
	SG_opt_writesTo                     SG_size* length,
	SG_opt_writesTo_withLength(length)  SG_indice* indices
);

SG_API_EXPORT
enum sg_status
sg_cube_vertices(
	SG_readsFrom                        struct sg_cube_info* info,
	SG_opt_writesTo                     SG_size* length,
	SG_opt_writesTo_withLength(length)  struct sg_position* positions,
	SG_opt_writesTo_withLength(length)  struct sg_normal* normals,
	SG_opt_writesTo_withLength(length)  struct sg_texcoord* texcoords
);

SG_API_EXPORT
enum sg_status
sg_indexed_sphere_vertices(
	SG_readsFrom                        struct sg_indexed_sphere_info* info,
	SG_opt_writesTo                     SG_size* length,
	SG_opt_writesTo_withLength(length)  struct sg_position* positions,
	SG_opt_writesTo_withLength(length)  struct sg_normal* normals,
	SG_opt_writesTo_withLength(length)  struct sg_texcoord* texcoords
);

SG_API_EXPORT
enum sg_status
sg_indexed_sphere_indices(
	SG_readsFrom                        struct sg_indexed_sphere_info* info,
	SG_opt_writesTo                     SG_size* length,
	SG_opt_writesTo_withLength(length)  SG_indice* indices
);
	

/** ***********************************************************
 * Math
 */

#ifndef	SIMPLE_GEOMETRY_DONT_INCLUDE_MATH_H
#  include <math.h>
#  define SG_SQUARE_ROOT(V) sqrt(V)
#  define SG_COS(V) cos(V)
#  define SG_SIN(V) sin(V)
#endif
	
#ifndef SG_PI
#  define SG_PI 3.1415926535897932384626433832795f
#endif	

#define SG_2PI (2.0f * SG_PI)

#define SG_EXPAND_XYZ(T) (T.x), (T.y), (T.z)

struct sg_vec3f {
	SG_float x;
	SG_float y;
	SG_float z;
};
	
SG_API_EXPORT
struct sg_vec3f
sg_vec3f_subtract(SG_uses const struct sg_vec3f a,
				  SG_uses const struct sg_vec3f b);

SG_API_EXPORT
struct sg_vec3f
sg_vec3f_dividef(SG_uses const struct sg_vec3f a,
				 SG_uses const SG_float f);

SG_API_EXPORT
struct sg_vec3f
sg_vec3f_cross(SG_uses const struct sg_vec3f a,
			   SG_uses const struct sg_vec3f b);

SG_API_EXPORT
SG_float
sg_vec3f_length(SG_uses const struct sg_vec3f a);

SG_API_EXPORT
struct sg_vec3f
sg_vec3f_normalize(SG_uses const struct sg_vec3f a);

SG_API_EXPORT
struct sg_vec3f
sg_vec3f_from_position(SG_uses const struct sg_position pos);

SG_API_EXPORT
struct sg_normal
sg_normal_from_vec3f(SG_uses const struct sg_vec3f v);
	

/** ***********************************************************
 * Materials
 */

struct sg_material {
	struct sg_vec3f ambient;
	struct sg_vec3f diffuse;
	struct sg_vec3f specular;
	SG_float shininess;
};

SG_API_EXPORT
struct sg_material
sg_material_gold(); 

SG_API_EXPORT
struct sg_material
sg_material_obsidian(); 
	
SG_API_EXPORT
struct sg_material
sg_material_ruby(); 

SG_API_EXPORT
struct sg_material
sg_material_emerald(); 

SG_API_EXPORT
struct sg_material
sg_material_default_flat_white();
	


/** ***********************************************************
 * Implementation
 */
#ifdef SIMPLE_GEOMETRY_IMPLEMENTATION

SG_bool
sg_success(SG_uses const enum sg_status status)
{
	switch (status) {
	case SG_OK_RETURNED_BUFFER:
	case SG_OK_RETURNED_LENGTH:
	case SG_OK_COPIED_TO_DST:
		return SG_true;
	default:
		return SG_false;
	};
}
	
const char*
sg_status_string(SG_uses const enum sg_status status)
{
	switch (status) {
	case SG_OK_RETURNED_BUFFER:                      return "SG_OK_RETURNED_BUFFER"; 
	case SG_OK_RETURNED_LENGTH:                      return "SG_OK_RETURNED_LENGTH";
	case SG_OK_COPIED_TO_DST:                        return "SG_OK_COPIED_TO_DST";
	case SG_ERR_NULLPTR_INPUT:                       return "SG_ERR_NULLPTR_INPUT";
	case SG_ERR_ZEROSIZE_INPUT:                      return "SG_ERR_ZEROSIZE_INPUT";
	case SG_ERR_INFO_NOT_PROVIDED:                   return "SG_ERR_INFO_NOT_PROVIDED";
	case SG_ERR_SUBDIVISIONS_MUST_BE_GREATER_THAN_1: 
		return "SG_ERR_SUBDIVISIONS_MUST_BE_GREATER_THAN_1";
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
	SG_readsFrom                   const void* src,
	SG_uses                        const SG_size n,
	SG_opt_writesTo_withLength(n)  void* dst
)
{
	SG_size i;
	if (src == SG_nullptr || dst == SG_nullptr)
		return SG_ERR_NULLPTR_INPUT;
	if (n < 1)
		return SG_ERR_ZEROSIZE_INPUT;

	for (i = 0; i < n; i++)
		((char*)dst)[i] = ((char*)src)[i];
	return SG_OK_COPIED_TO_DST;
}
	
struct sg_strided_blockcopy_source_info {
	void* ptr;
	SG_size block_size;
	SG_size block_count;
	SG_size stride;
};

enum sg_status
sg_strided_blockcopy(
	SG_readsFrom     struct sg_strided_blockcopy_source_info const* source,
	SG_uses          const SG_size stride,
	SG_opt_writesTo  void* dst
)
{
	SG_size i;
	char* currsrc;
	char* currdst;
	enum sg_status copystatus;

	if (source == SG_nullptr)
		return SG_ERR_INFO_NOT_PROVIDED;
	if (source->ptr == SG_nullptr || dst == SG_nullptr)
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
sg_vec3f_dividef(const struct sg_vec3f a, const SG_float f)
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

SG_float
sg_vec3f_length(const struct sg_vec3f a)
{
	return SG_SQUARE_ROOT(a.x*a.x + a.y*a.y + a.z*a.z);
}

struct sg_vec3f
sg_vec3f_normalize(const struct sg_vec3f a)
{
	SG_float length;
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
sg_face_normal(SG_uses struct sg_position p1,
			   SG_uses struct sg_position p2,
			   SG_uses struct sg_position p3)
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
	SG_readsFrom                                 const struct sg_position* vertices,
	SG_uses                                      const SG_size vertices_length,
	SG_opt_writesTo_withLength(vertices_length)  struct sg_normal* normals
)
{
	struct sg_normal normal;
	SG_size i;

	if (vertices == SG_nullptr || normals == SG_nullptr)
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
	SG_readsFrom                        struct sg_indexed_plane_info const* plane,
	SG_opt_writesTo                     SG_size* length,
	SG_opt_writesTo_withLength(length)  struct sg_position* positions,
	SG_opt_writesTo_withLength(length)  struct sg_normal* normals,
	SG_opt_writesTo_withLength(length)  struct sg_texcoord* texcoords
)
{
	if (plane == SG_nullptr)
		return SG_ERR_INFO_NOT_PROVIDED;
	if (length == SG_nullptr)
		return SG_ERR_DSTLEN_NOT_PROVIDED;
	if (plane->width_subdivisions < 1 || plane->depth_subdivisions < 1)
		return SG_ERR_SUBDIVISIONS_MUST_BE_GREATER_THAN_1;

	if (positions == SG_nullptr && normals == SG_nullptr && texcoords == SG_nullptr) {
		*length = plane->width_subdivisions * plane->depth_subdivisions;
		return SG_OK_RETURNED_LENGTH;
	}
	
	for (SG_size w = 0; w < plane->width_subdivisions; w++) {
		for (SG_size h = 0; h < plane->depth_subdivisions; h++) {
			//TODO: it might be we need depth_subdivisions here
			SG_size index = h + (w * plane->width_subdivisions);
			SG_float width_scale = plane->width / plane->width_subdivisions;
			SG_float depth_scale = plane->depth / plane->depth_subdivisions;
			if (positions != SG_nullptr) {
				sg_position position {
					.x = width_scale * w,
					.y = depth_scale * h,
					.z = 0
				};
				positions[index] = position;
			}

			if (normals != SG_nullptr) {
				sg_normal normal {
					.x = 0,
					.y = 0,
					.z = 1 
				};
				normals[index] = normal;
			}

			if (texcoords != SG_nullptr) {
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
	SG_readsFrom                        struct sg_indexed_plane_info const* plane,
	SG_opt_writesTo                     SG_size* length,
	SG_opt_writesTo_withLength(length)  SG_indice* indices
)
{
	SG_UNREFERENCED(plane);
	SG_UNREFERENCED(length);
	SG_UNREFERENCED(indices);
	return SG_ERR_NOT_IMPLEMENTED_YET;
}


enum sg_status
sg_cube_vertices(
	SG_readsFrom                        struct sg_cube_info* info,
	SG_opt_writesTo                     SG_size* length,
	SG_opt_writesTo_withLength(length)  struct sg_position* positions,
	SG_opt_writesTo_withLength(length)  struct sg_normal* normals,
	SG_opt_writesTo_withLength(length)  struct sg_texcoord* texcoords
)
{
	const struct sg_position _positions[] = {
		{-info->width, -info->height, -info->depth},
		{ info->width, -info->height, -info->depth},
		{ info->width,  info->height, -info->depth},
		{ info->width,  info->height, -info->depth},
		{-info->width,  info->height, -info->depth},
		{-info->width, -info->height, -info->depth},

		{-info->width, -info->height,  info->depth},
		{ info->width, -info->height,  info->depth},
		{ info->width,  info->height,  info->depth},
		{ info->width,  info->height,  info->depth}, 
		{-info->width,  info->height,  info->depth},
		{-info->width, -info->height,  info->depth},

		{-info->width,  info->height,  info->depth},
		{-info->width,  info->height, -info->depth},
		{-info->width, -info->height, -info->depth},
		{-info->width, -info->height, -info->depth},
		{-info->width, -info->height,  info->depth},
		{-info->width,  info->height,  info->depth},

		{ info->width,  info->height,  info->depth},
		{ info->width,  info->height, -info->depth},
		{ info->width, -info->height, -info->depth},
		{ info->width, -info->height, -info->depth},
		{ info->width, -info->height,  info->depth},
		{ info->width,  info->height,  info->depth},

		{-info->width, -info->height, -info->depth},
		{ info->width, -info->height, -info->depth},
		{ info->width, -info->height,  info->depth},
		{ info->width, -info->height,  info->depth},
		{-info->width, -info->height,  info->depth},
		{-info->width, -info->height, -info->depth},

		{-info->width,  info->height, -info->depth},
		{ info->width,  info->height, -info->depth},
		{ info->width,  info->height,  info->depth},
		{ info->width,  info->height,  info->depth},
		{-info->width,  info->height,  info->depth},
		{-info->width,  info->height, -info->depth},
	};
	const struct sg_normal _normals[] = {
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
	const struct sg_texcoord _texcoords[] = {
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
		{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},
		
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
		{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},
		
		{1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
		{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},
		
		{1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
		{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},
		
		{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
		{1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f},
		
		{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
		{1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}
	};
	SG_size i;
	
	if (info == SG_nullptr)
		return SG_ERR_INFO_NOT_PROVIDED;

	if (length == SG_nullptr)
		return SG_ERR_DSTLEN_NOT_PROVIDED;

	if (positions == SG_nullptr && normals == SG_nullptr && texcoords == SG_nullptr) {
		*length = 36;
		return SG_OK_RETURNED_LENGTH;
	}

	if (positions != SG_nullptr)
		for (i = 0; i < 36; ++i)
			positions[i] = _positions[i];

	if (normals != SG_nullptr)
		for (i = 0; i < 36; ++i)
			normals[i] = _normals[i];

	if (texcoords != SG_nullptr)
		for (i = 0; i < 36; ++i)
			texcoords[i] = _texcoords[i];

	return SG_OK_RETURNED_BUFFER;
}



enum sg_status
sg_indexed_sphere_vertices(
	SG_readsFrom                        struct sg_indexed_sphere_info* info,
	SG_opt_writesTo                     SG_size* length,
	SG_opt_writesTo_withLength(length)  struct sg_position* positions,
	SG_opt_writesTo_withLength(length)  struct sg_normal* normals,
	SG_opt_writesTo_withLength(length)  struct sg_texcoord* texcoords
)
/**
 * Original Reference:
 * https://www.3dgep.com/texturing-and-lighting-with-opengl-and-glsl/#Creating_a_Sphere
 */
{
	if (info == SG_nullptr)
		return SG_ERR_INFO_NOT_PROVIDED;

	if (length == SG_nullptr)
		return SG_ERR_DSTLEN_NOT_PROVIDED;

	if (positions == SG_nullptr && normals == SG_nullptr && texcoords == SG_nullptr) {
		*length = (info->slices+1)*(info->stacks+1);
		return SG_OK_RETURNED_LENGTH;
	}

	SG_size n = 0;
	for (SG_size i = 0; i <= info->stacks; ++i) {
		sg_texcoord texcoord{
			.u = 0,
			.v = 0
		};

		texcoord.v = i / (SG_float)info->stacks;
		SG_float phi = texcoord.v * SG_PI;

		for (SG_size j = 0; j <= info->slices; ++j) {
			texcoord.u = j / (SG_float)info->slices;
			SG_float theta = texcoord.u * SG_2PI;
			sg_normal normal{
				.x = SG_COS(theta) * SG_SIN(phi),
				.y = SG_COS(phi),
				.z = SG_SIN(theta) * SG_SIN(phi)
			};

			if (normals != SG_nullptr)
				normals[n] = normal;

			if (positions != SG_nullptr) {
				sg_position position{
					.x = normal.x * info->radius,
					.y = normal.y * info->radius,
					.z = normal.z * info->radius
				};
				positions[n] = position;
			}
			
			if (texcoords != SG_nullptr)
				texcoords[n] = texcoord;
			n++;
		}
	}

	return SG_OK_RETURNED_BUFFER;
}

enum sg_status
sg_indexed_sphere_indices(
	SG_readsFrom                        struct sg_indexed_sphere_info* info,
	SG_opt_writesTo                     SG_size* length,
	SG_opt_writesTo_withLength(length)  SG_indice* indices
)
{
	if (info == SG_nullptr)
		return SG_ERR_INFO_NOT_PROVIDED;

	if (length == SG_nullptr)
		return SG_ERR_DSTLEN_NOT_PROVIDED;
	
	if (indices == SG_nullptr) {
		*length = (info->slices * info->stacks + info->slices) * 6;
		return SG_OK_RETURNED_LENGTH;
	}
	
	SG_size n = 0;
	for (SG_size i = 0; i < info->slices * info->stacks + info->slices; ++i) {
		indices[n++] = i;
		indices[n++] = i + info->slices + 1;
		indices[n++] = i + info->slices;

		indices[n++] = i + info->slices + 1;
		indices[n++] = i;
		indices[n++] = i + 1;
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

