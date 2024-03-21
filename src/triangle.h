#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"

typedef struct {
	int a;
	int b;
	int c;
} face_t; // stores vertex indices for each face/triangle of a mesh

typedef struct {
	vec2_t points[3];
} triangle_t; // stores the actual vec2 points of the triangle in the screen

#endif
