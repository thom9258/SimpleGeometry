#define SIMPLE_GEOMETRY_IMPLEMENTATION
#define	SIMPLE_GEOMETRY_DONT_INCLUDE_MATH_H
#include <math.h>
#define SG_SQUARE_ROOT(DOUB) sqrt(DOUB)
#include "../simple_geometry.h"

#include <malloc.h>
#include <stdio.h>
#include <assert.h>

void test_data_copying()
{
	char src[] = "this is my ascii string";
	char dst[SG_ARRAY_LEN(src)] = {' '};
	enum sg_status status = SG_OK_RETURNED_BUFFER;

	status = sg_memcpy(SG_nullptr, 27, dst);
	assert(!sg_success(status) && "invalid src was accepted");
	status = sg_memcpy(src, 0, dst);
	assert(!sg_success(status) && "invalid size was accepted");
	status = sg_memcpy(src, 27, SG_nullptr);
	assert(!sg_success(status) && "invalid dst was accepted");

	status = sg_memcpy(src, SG_ARRAY_LEN(src), dst);
	assert(sg_success(status) && "could not copy buffer");
	
	printf("src: '%s'\ndst:'%s'\n", src, dst);
}

void test_strided_data_copying_errs()
{
	char src1[] = "12121212";
	char dst[] = "                       ";
	enum sg_status status = SG_OK_RETURNED_BUFFER;

	//status = sg_strided_blockcopy(0, 2, 4, src1, 5, dst);
	//assert(!sg_success(status) && "was allowed to pass invalid srcblksize");
	//status = sg_strided_blockcopy(2, 2, 0, src1, 5, dst);
	//assert(!sg_success(status) && "was allowed to pass invalid blkcount");
	//status = sg_strided_blockcopy(2, 2, 4, SG_nullptr, 5, dst);
	//assert(!sg_success(status) && "was allowed to pass invalid srcptr");
	//status = sg_strided_blockcopy(2, 2, 4, src1, 0, dst);
	//assert(!sg_success(status) && "was allowed to pass invalid dststride");
	//status = sg_strided_blockcopy(2, 2, 4, src1, 5, SG_nullptr);
	//assert(!sg_success(status) && "was allowed to pass invalid dstptr");
}

void test_strided_data_copying()
{
	char src1[] = "12121212";
	char src2[] = "345";
	char dst[] = "                       ";
	enum sg_status status = SG_OK_RETURNED_BUFFER;

	struct sg_strided_blockcopy_info copy_info;
	copy_info.src.ptr = src1;
	copy_info.src.block_size = 2;
	copy_info.src.stride = 2;
	copy_info.src.block_count = 4;
	copy_info.dst.stride = 5;

	status = sg_strided_blockcopy(&copy_info, dst);
	assert(sg_success(status) && "could not copy buffer");

	copy_info.src.ptr = src2;
	copy_info.src.block_size = 3;
	copy_info.src.stride = 0;
	copy_info.src.block_count = 4;
	copy_info.dst.stride = 5;

	status = sg_strided_blockcopy(&copy_info, dst + 2);
	assert(sg_success(status) && "could not copy buffer");
	
	printf("src1: '%s'\nsrc2: '%s'\ndst: '%s'\n", src1, src2, dst);
}

void get_plane_positions()
{
	unsigned int size = 0;
	enum sg_status status = SG_OK_RETURNED_BUFFER;
	struct sg_position* pos = NULL;
	unsigned int i;

	status = sg_plane_positions(2.0f, 1.5f, NULL, NULL);
	assert(!sg_success(status) && "Not an error to not provide either buffer or size dsts");

	status = sg_plane_positions(2.0f, 1.5f, NULL, &size);
	assert(sg_success(status) && "Could not get size of plane vertice positions");
	printf("plane position count %i\n", size);
	
	pos = (struct sg_position*)malloc(sizeof(struct sg_position) * size);
	assert(pos != NULL && "Could not plane vertice positions buffer");
	printf("plane position buffer ptr %p\n", pos);

	status = sg_plane_positions(2.0f, 1.5f, pos, &size);
	assert(sg_success(status) && "Could not get plane vertice positions");
	
	printf("Plane Vertex Positions:\n");
	for (i = 0; i < size; ++i) {
		struct sg_position curr = pos[i];
		printf("%i\t%f %f %f\n", i, curr.x, curr.y, curr.z);
	}
}

void get_cube_positions()
{
	unsigned int size = 0;
	enum sg_status status = SG_OK_RETURNED_BUFFER;
	struct sg_position* pos = NULL;
	unsigned int i;

	status = sg_cube_positions(0, 0, 0, NULL, NULL);
	assert(!sg_success(status) && "Not an error to not provide either buffer or size dsts");

	status = sg_cube_positions(2.0f, 1.5f, 10.0f, NULL, &size);
	assert(sg_success(status) && "Could not get size of cube vertice positions");
	printf("Cube position count %i\n", size);
	
	pos = (struct sg_position*)malloc(sizeof(struct sg_position) * size);
	assert(pos != NULL && "Could not cube vertice positions buffer");
	printf("Cube position buffer ptr %p\n", pos);

	status = sg_cube_positions(2.0f, 1.5f, 10.0f, pos, &size);
	assert(sg_success(status) && "Could not get cube vertice positions");
	
	printf("Cube Vertex Positions:\n");
	for (i = 0; i < size; ++i) {
		struct sg_position curr = pos[i];
		printf("%i\t%f %f %f\n", i, curr.x, curr.y, curr.z);
	}
}

int main(int argc, char** argv)
{
	test_data_copying();
	test_strided_data_copying_errs();
	test_strided_data_copying();
	get_plane_positions();
	get_cube_positions();
}
