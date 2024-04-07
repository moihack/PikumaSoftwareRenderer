#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"

typedef struct {
	int a;
	int b;
	int c;
} face_t; // stores vertex indices for each face/triangle of a mesh

typedef struct {
	vec2_t points[3];
} triangle_t; // stores the actual vec2 points of the triangle in the screen

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

#endif
