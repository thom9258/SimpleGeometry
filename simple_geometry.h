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
	
#ifndef	SIMPLE_GEOMETRY_DONT_INCLUDE_ASSERT_H
#  include <assert.h>
#  ifndef SG_ASSERT_NOT_IMPLEMENTED
#    define SG_ASSERT_NOT_IMPLEMENTED(NAMESTR) assert(1 && "FUNCTION [" NAMESTR "] HAS NOT BEEN IMPLEMENTED YET!")
#  endif
#endif

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

#ifndef	SG_UNREFERENCED
#  define SG_UNREFERENCED(VAR) (void)(VAR)
#endif
	
#ifndef	SG_IN
#  define SG_IN
#endif
#ifndef	SG_OUT
#  define SG_OUT
#endif
#ifndef	SG_INOUT
#  define SG_INOUT
#endif
	
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
	
#define	SG_ARRAY_MEMSIZE(ARR) sizeof(ARR)
#define	SG_ARRAY_LEN(ARR) (SG_ARRAY_MEMSIZE(ARR) / sizeof(ARR[0]))

enum sg_status {
	SG_OK_RETURNED_BUFFER,
	SG_OK_RETURNED_LEN,
	SG_OK_COPIED_TO_DST,

	SG_ERR_NULLPTR_INPUT,
	SG_ERR_ZEROSIZE_INPUT,
	SG_ERR_DSTLEN_NOT_PROVIDED,
	SG_ERR_SRCBLKSIZE_LESSTHAN_SRCSTRIDE,
	SG_ERR_SRCBLKSIZE_LESSTHAN_DSTSTRIDE,
	SG_ERR_VERTICES_NOT_DIVISIBLE_BY_3,

	SG_ERR_NOT_IMPLEMENTED_YET,
};
	
struct sg_vec3f {
	SG_float x;
	SG_float y;
	SG_float z;
};
	
#define SG_EXPAND_XYZ(T) (T.x), (T.y), (T.z)
	
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
	
struct sg_material {
	struct sg_vec3f ambient;
	struct sg_vec3f diffuse;
	struct sg_vec3f specular;
	SG_float shininess;
};

/**
 * @brief Check if returned SG_STATUS indicates success.
 *
 * A simplified summary of the return code, if ignoring any
 * specific error/success code is wanted.
 */
SG_API_EXPORT
SG_bool
sg_success(SG_IN const enum sg_status status); 
	
/**
 * @brief Check if returned SG_STATUS indicates success.
 *
 * A simplified summary of the return code, if ignoring any
 * specific error/success code is wanted.
 */

SG_API_EXPORT
const char*
sg_status_string(SG_IN const enum sg_status status);
	
/**
 * @brief Copy memory.
 *
 * Copies 'n' bytes of memory from 'src' to 'dst'.
 */
SG_API_EXPORT
enum sg_status
sg_memcpy(SG_IN const void* src,
		  SG_IN const SG_size n,
		  SG_OUT void* dst);

/**
 * @brief Copy strided blocks of memory.
 *
 * @param  src_block_size   individual block copy size of source data.
 * @param  src_stride       The offset size for each block to copy.
 * @param  src_block_count  The amount of blocks to copy.
 * @param  src              The buffer to copy blocks from.
 * @param  dst_stride       The offset size of each copied block at destination.
 * @param  dst              The buffer to copy blocks to.
 *
 * Copies 'src_block_count' blocks from 'src' of 'src_block_size'
 * with 'src_stride' to specified 'dst' with specified 'dst_stride'.
 *
 * This function allows interlacing of different buffers into a single,
 * coherent buffer, converting multiple Structure of Arrays (SoA)'s into a
 * single Array of Structures (AoS) buffer.
 *
 * @note if 'src_stride' is zero, this function can be used 
 *       as a block duplication mechanism.
 *
 * @return The result status of the copying.
 * 
 * @example
 * Observe a 'SoA' pos buffer & its texcoord buffer:
 *  xyzType XYZ[] = [xyz1][xyz2][xyz3]
 *  uvType UV[] = [uv1][uv2][uv3]
 *
 * interlacing using strided blockcopying could create an 'AoS' buffer:
 *   [xyz1][uv1][xyz2][uv2][xyz3][uv3]
 *
 * The function signature is complex, but in order to create the 'AoS'
 * buffer above, the process would be:
 *
 *   NewBuffer[] = ...
 *
 * A suitable buffer needs to be prepared, with enough memory to fit
 * all the interlazed buffers.
 * Then, we start by copying the first buffer:
 *
 *   sg_strided_blockcopy(sizeof(xyzType),
 *                        sizeof(xyzType),
 *                        3,
 *                        XYZ,
 *                        sizeof(xyzType) + sizeof(uvType),
 *                        NewBuffer);
 *
 * Here it should be noted that the source buffer has its data packed
 * linearly, so the source stride is just the size of the individual elements.
 * Similarly, in the new buffer we would need to fit a element from each
 * buffer alongsize each-other, so the destination stride is the cumulative
 * size of each element.
 *
 * Now we copy over the second buffer:
 *
 *   sg_strided_blockcopy(sizeof(uvType),
 *                        sizeof(uvType),
 *                        3,
 *                        UV,
 *                        sizeof(xyzType) + sizeof(uvType),
 *                        NewBuffer + sizeof(xyzType));
 *
 * Here it is imperative to remember that the destination pointer needs to be
 * offset with the size of the other buffer elements already placed, so
 * that we do not overwrite them.
 * Other than this, the inputs follow a similar structure as all the
 * previous strided copying operations. 
 */
SG_API_EXPORT
enum sg_status
sg_strided_blockcopy(SG_IN  const SG_size src_block_size,
					 SG_IN  const SG_size src_stride,
					 SG_IN  const SG_size src_block_count,
					 SG_IN  const void* src,
					 SG_IN  const SG_size dst_stride,
					 SG_OUT void* dst);
	
SG_API_EXPORT
struct sg_vec3f
sg_vec3f_subtract(SG_IN const struct sg_vec3f a,
				  SG_IN const struct sg_vec3f b);

SG_API_EXPORT
struct sg_vec3f
sg_vec3f_dividef(SG_IN const struct sg_vec3f a,
				 SG_IN const SG_float f);

SG_API_EXPORT
struct sg_vec3f
sg_vec3f_cross(SG_IN const struct sg_vec3f a,
			   SG_IN const struct sg_vec3f b);

SG_API_EXPORT
SG_float
sg_vec3f_length(SG_IN const struct sg_vec3f a);

SG_API_EXPORT
struct sg_vec3f
sg_vec3f_normalize(SG_IN const struct sg_vec3f a);

SG_API_EXPORT
struct sg_vec3f
sg_vec3f_from_position(SG_IN const struct sg_position pos);

SG_API_EXPORT
struct sg_normal
sg_normal_from_vec3f(SG_IN const struct sg_vec3f v);
	
/**
 * @brief Calculate flat normals for vertices.
 */
SG_API_EXPORT
enum sg_status
sg_calculate_flat_normals(SG_IN  const struct sg_position* vertices,
						  SG_IN  const SG_size vertices_len,
						  SG_OUT struct sg_normal* normals);


/**
 * @brief Get Plane Position Vertices.
 *
 * Call Signature 1:
 * To get the length of the required memory buffer, call this
 * function with 'dst' set to SG_nullptr.
 * When this is done, specified 'width' & 'height' is unused.

 * Call Signature 2:
 * To get the vertices, provide a buffer of *atleast* the
 * returned length as gotten when using call signature 1.
 * the provided 'width' & 'height' is used to calculate the
 * size of the returned plane.
 *
 * @return a return code indicating the status of the use.
 */
SG_API_EXPORT
enum sg_status
sg_plane_positions(SG_IN  const SG_float width,
				   SG_IN  const SG_float height,
				   SG_OUT struct sg_position* dst,
				   SG_OUT SG_size* dstlen);


SG_API_EXPORT
enum sg_status
sg_plane_texcoords(SG_OUT struct sg_texcoord* dst,
				   SG_OUT SG_size* dstlen);
	
SG_API_EXPORT
enum sg_status
sg_plane(SG_IN  const SG_float width,
		 SG_IN  const SG_float height,
		 SG_OUT SG_size* length,
		 SG_OUT struct sg_position* positions,
		 SG_OUT struct sg_texcoord* texcoords);
	
/**
 * @brief Get Cube Position Vertices.
 *
 * @param width      Width of the returned cube.
 * @param height     Height of the returned cube.
 * @param depth      Depth of the returned cube.
 * @param length     Pointer to the output minimim length of 
 *                   'positions', 'normals' & 'texcoords' buffers.
 * @param positions  Pointer to the output position buffer to fill.
 * @param normals    Pointer to the output normal buffer to fill.
 * @param texcoords  Pointer to the output texcoord buffer to fill.
 *
 * Call Signature 1:
 *   To get the minimum length of the output buffers.
 *   call this function with 'positions', 'normals' & 'texcoords'
 *   buffers set to 'SG_nullptr'.
 *
 * Call Signature 2:
 *   To get 'positions', 'normals' & 'texcoords', provide buffers of 
 *   *at-least* the returned 'dstlen' as is retrieved when using
 *   call signature 1.
 *   the provided 'width', 'height' & 'depth' is used to specify the
 *   dimensions of the returned cube.
 *
 * @see struct sg_position
 * @see struct sg_normal
 * @see struct sg_texcoord
 *
 * @return a return code indicating the status of the usage.
 */
SG_API_EXPORT
enum sg_status
sg_cube(SG_IN  const SG_float width,
		SG_IN  const SG_float height,
		SG_IN  const SG_float depth,
		SG_OUT SG_size* length,
		SG_OUT struct sg_position* positions,
		SG_OUT struct sg_normal* normals,
		SG_OUT struct sg_texcoord* texcoords);

/**
 * @brief Get indexed sphere vertices.
 *
 * Call Signature 1:
 * To get the length of the required memory buffers, point 'len'
 * to a valid variable, then set 'positions', 'normals' & 'texcoords'
 * to 'SG_nullptr'.
 * When this is done, ensure specified 'slices' & 'stacks' are set
 * as these determine the returned length.
 *
 * Call Signature 2:
 * To get the vertices, provide 'positions', 'normals' & 'texcoords'
 * buffers of *atleast* the returned length as gotten when using
 * call signature 1.
 * If any of the returnable vertex data buffers are not needed,
 * provide 'SG_nullptr' instead.
 *
 * Original Reference:
 * https://www.3dgep.com/texturing-and-lighting-with-opengl-and-glsl/#Creating_a_Sphere
 *
 * @return a return code indicating the status of the use.
 */
SG_API_EXPORT
enum sg_status
sg_indexed_sphere(SG_IN  const SG_float radius,
				  SG_IN  const SG_size slices,
				  SG_IN  const SG_size stacks,
				  SG_OUT SG_size* length,
				  SG_OUT struct sg_position* positions,
				  SG_OUT struct sg_normal* normals,
				  SG_OUT struct sg_texcoord* texcoords);


/**
 * @brief Get indexed sphere vertices.
 *
 * Call Signature 1:
 * To get the length of the required memory buffer, call this
 * function with 'len' pointing to a valid value, and 'indices'
 * pointint to 'SG_nullptr'.
 * When this is done, ensure specified 'slices' & 'stacks' are
 * set as these determine the returned length.
 *
 * Call Signature 2:
 * To get the 'indices' provide a buffer of *atleast* the
 * returned length as gotten when using call signature 1.
 *
 * Original Reference:
 * https://www.3dgep.com/texturing-and-lighting-with-opengl-and-glsl/#Creating_a_Sphere
 *
 * @return a return code indicating the status of the use.
 */
SG_API_EXPORT
enum sg_status
sg_indexed_sphere_indices(SG_IN  const SG_size slices,
						  SG_IN  const SG_size stacks,
						  SG_OUT SG_size* len,
						  SG_OUT SG_indice* indices);


/**
 * @brief Get Material properties for 'gold'.
 */
SG_API_EXPORT
struct sg_material
sg_material_gold(); 

/**
 * @brief Get Material properties for 'obsidian'.
 */
SG_API_EXPORT
struct sg_material
sg_material_obsidian(); 
	
/**
 * @brief Get Material properties for 'ruby'.
 */
SG_API_EXPORT
struct sg_material
sg_material_ruby(); 

/**
 * @brief Get Material properties for 'emerald'.
 */
SG_API_EXPORT
struct sg_material
sg_material_emerald(); 


	
/**
 * @brief Get Material properties for 'default flat white'.
 * This color emulates the default white color material for
 * for primitive objects used in common editors and engines.
 */
SG_API_EXPORT
struct sg_material
sg_material_default_flat_white();
	

/** ***********************************************************
 * Implementation
 */
#ifdef SIMPLE_GEOMETRY_IMPLEMENTATION

SG_bool
sg_success(SG_IN const enum sg_status status)
{
	switch (status) {
	case SG_OK_RETURNED_BUFFER:
	case SG_OK_RETURNED_LEN:
	case SG_OK_COPIED_TO_DST:
		return SG_true;
	default:
		return SG_false;
	};
}
	
const char*
sg_status_string(SG_IN const enum sg_status status)
{
	switch (status) {
	case SG_OK_RETURNED_BUFFER:                return "SG_OK_RETURNED_BUFFER"; 
	case SG_OK_RETURNED_LEN:                   return "SG_OK_RETURNED_LEN";
	case SG_OK_COPIED_TO_DST:                  return "SG_OK_COPIED_TO_DST";
	case SG_ERR_NULLPTR_INPUT:                 return "SG_ERR_NULLPTR_INPUT";
	case SG_ERR_ZEROSIZE_INPUT:                return "SG_ERR_ZEROSIZE_INPUT";
	case SG_ERR_DSTLEN_NOT_PROVIDED:           return "SG_ERR_DSTLEN_NOT_PROVIDED";
	case SG_ERR_SRCBLKSIZE_LESSTHAN_SRCSTRIDE: return "SG_ERR_SRCBLKSIZE_LESSTHAN_SRCSTRIDE";
	case SG_ERR_SRCBLKSIZE_LESSTHAN_DSTSTRIDE: return "SG_ERR_SRCBLKSIZE_LESSTHAN_DSTSTRIDE";
	case SG_ERR_NOT_IMPLEMENTED_YET:           return "SG_ERR_NOT_IMPLEMENTED_YET";
	case SG_ERR_VERTICES_NOT_DIVISIBLE_BY_3:   return "SG_ERR_VERTICES_NOT_DIVISIBLE_BY_3";
	};
	return "SG_UNKNOWN_STATUS";
}

enum sg_status
sg_memcpy(SG_IN  const void* src,
		  SG_IN  const SG_size n,
		  SG_OUT void* dst)
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

enum sg_status
sg_strided_blockcopy(SG_IN  const SG_size src_block_size,
					 SG_IN  const SG_size src_stride,
					 SG_IN  const SG_size src_block_count,
					 SG_IN  const void* src,
					 SG_IN  const SG_size dst_stride,
					 SG_OUT void* dst)
{
	SG_size i;
	char* currsrc;
	char* currdst;
	enum sg_status copystatus;
	if (src == SG_nullptr || dst == SG_nullptr)
		return SG_ERR_NULLPTR_INPUT;
	if (dst_stride < 1 || src_block_size < 1 || src_block_count < 1)
		return SG_ERR_ZEROSIZE_INPUT;
	if (dst_stride < src_block_size)
		return SG_ERR_SRCBLKSIZE_LESSTHAN_DSTSTRIDE;
	
	for (i = 0; i < src_block_count; i++) {
		currsrc = ((char*)src) + (i * src_stride);
		currdst = ((char*)dst) + (i * dst_stride);
		copystatus = sg_memcpy(currsrc, src_block_size, currdst);
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
sg_face_normal(SG_IN struct sg_position p1,
			   SG_IN struct sg_position p2,
			   SG_IN struct sg_position p3)
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
sg_calculate_flat_normals(SG_IN  const struct sg_position* vertices,
						  SG_IN  const SG_size vertices_len,
						  SG_OUT struct sg_normal* normals)
{
	struct sg_normal normal;
	SG_size i;

	if (vertices == SG_nullptr || normals == SG_nullptr)
		return SG_ERR_NULLPTR_INPUT;
	if ((vertices_len % 3) != 0)
		return SG_ERR_VERTICES_NOT_DIVISIBLE_BY_3;

	for (i = 0; i < vertices_len; i += 3) {
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
sg_plane_positions(SG_IN  const SG_float width,
				   SG_IN  const SG_float height,
				   SG_OUT struct sg_position* dst,
				   SG_OUT SG_size* dstlen)
{
	const struct sg_position positions[] = {
        {-width, -height, 0.0f},
        { width, -height, 0.0f},
        { width,  height, 0.0f},
		
        { width,  height, 0.0f},
        {-width,  height, 0.0f},
        {-width, -height, 0.0f}
    };

	if (dstlen == SG_nullptr)
		return SG_ERR_DSTLEN_NOT_PROVIDED;

	*dstlen = SG_ARRAY_LEN(positions);
	if (dst == SG_nullptr)
		return SG_OK_RETURNED_LEN;
	
	return sg_memcpy(positions,
					 SG_ARRAY_MEMSIZE(positions),
					 dst);
}

enum sg_status
sg_plane_texcoords(SG_OUT struct sg_texcoord* dst,
				   SG_OUT SG_size* dstlen)
{
	const struct sg_texcoord out[] = {
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{0.0f, 1.0f},

		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
	};
	const SG_size outlen = SG_ARRAY_LEN(out);
	SG_size i;
		if (dstlen == SG_nullptr)
		return SG_ERR_DSTLEN_NOT_PROVIDED;

	*dstlen = outlen;
	if (dst == SG_nullptr)
		return SG_OK_RETURNED_LEN;
	
	for (i = 0; i < outlen; ++i) {
		dst[i] = out[i];
	}

	return SG_OK_RETURNED_BUFFER;
}


enum sg_status
sg_plane(SG_IN  const SG_float width,
		 SG_IN  const SG_float height,
		 SG_OUT SG_size* length,
		 SG_OUT struct sg_position* positions,
		 SG_OUT struct sg_texcoord* texcoords)
{
	SG_ASSERT_NOT_IMPLEMENTED("sg_plane");
	SG_UNREFERENCED(width);
	SG_UNREFERENCED(height);
	SG_UNREFERENCED(length);
	SG_UNREFERENCED(positions);
	SG_UNREFERENCED(texcoords);
	return SG_ERR_NOT_IMPLEMENTED_YET;

}


enum sg_status
sg_cube(SG_IN  const SG_float width,
		SG_IN  const SG_float height,
		SG_IN  const SG_float depth,
		SG_OUT SG_size* length,
		SG_OUT struct sg_position* positions,
		SG_OUT struct sg_normal* normals,
		SG_OUT struct sg_texcoord* texcoords)
{
	const struct sg_position _positions[] = {
		{-width, -height, -depth}, { width, -height, -depth}, { width,  height, -depth},
		{ width,  height, -depth}, {-width,  height, -depth}, {-width, -height, -depth},

		{-width, -height,  depth}, { width, -height,  depth}, { width,  height,  depth},
		{ width,  height,  depth}, {-width,  height,  depth}, {-width, -height,  depth},

		{-width,  height,  depth}, {-width,  height, -depth}, {-width, -height, -depth},
		{-width, -height, -depth}, {-width, -height,  depth}, {-width,  height,  depth},

		{ width,  height,  depth}, { width,  height, -depth}, { width, -height, -depth},
		{ width, -height, -depth}, { width, -height,  depth}, { width,  height,  depth},

		{-width, -height, -depth}, { width, -height, -depth}, { width, -height,  depth},
		{ width, -height,  depth}, {-width, -height,  depth}, {-width, -height, -depth},

		{-width,  height, -depth}, { width,  height, -depth}, { width,  height,  depth},
		{ width,  height,  depth}, {-width,  height,  depth}, {-width,  height, -depth},
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

	if (length == SG_nullptr)
		return SG_ERR_DSTLEN_NOT_PROVIDED;

	if (positions == SG_nullptr && normals == SG_nullptr && texcoords == SG_nullptr) {
		*length = 36;
		return SG_OK_RETURNED_LEN;
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
sg_indexed_sphere(SG_IN  const SG_float radius,
				  SG_IN  const SG_size slices,
				  SG_IN  const SG_size stacks,
				  SG_OUT SG_size* length,
				  SG_OUT struct sg_position* positions,
				  SG_OUT struct sg_normal* normals,
				  SG_OUT struct sg_texcoord* texcoords)
{
	SG_size n;
	SG_size i;
	SG_size j;
	SG_float phi;
	SG_float theta;
	sg_normal normal;
	sg_texcoord texcoord;
	sg_position position;

	if (length == SG_nullptr)
		return SG_ERR_DSTLEN_NOT_PROVIDED;

	if (positions == SG_nullptr && normals == SG_nullptr && texcoords == SG_nullptr) {
		*length = (slices+1)*(stacks+1);
		return SG_OK_RETURNED_LEN;
	}

	n = 0;
	for (i = 0; i <= stacks; ++i) {
		texcoord.v = i / (SG_float)stacks;
		phi = texcoord.v * SG_PI;

		for (j = 0; j <= slices; ++j) {
			texcoord.u = j / (SG_float)slices;
			theta = texcoord.u * SG_2PI;

			normal.x = SG_COS(theta) * SG_SIN(phi);
			normal.y = SG_COS(phi);
			normal.z = SG_SIN(theta) * SG_SIN(phi);

			if (normals != SG_nullptr)
				normals[n] = normal;

			if (positions != SG_nullptr) {
				position.x = normal.x * radius;
				position.y = normal.y * radius;
				position.z = normal.z * radius;
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
sg_indexed_sphere_indices(SG_IN  const SG_size slices,
						  SG_IN  const SG_size stacks,
						  SG_OUT SG_size* len,
						  SG_OUT SG_indice* indices)
{
	SG_size i;
	SG_size n;
	if (len == SG_nullptr)
		return SG_ERR_DSTLEN_NOT_PROVIDED;
	
	if (indices == SG_nullptr) {
		*len = (slices * stacks + slices) * 6;
		return SG_OK_RETURNED_LEN;
	}
	
	n = 0;
	for (i = 0; i < slices * stacks + slices; ++i) {
		indices[n++] = i;
		indices[n++] = i + slices + 1;
		indices[n++] = i + slices;

		indices[n++] = i + slices + 1;
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

