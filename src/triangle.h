#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "texture.h"
#include "vector.h"

typedef struct {
	int a;
	int b;
	int c;
	tex2_t a_uv;
	tex2_t b_uv;
	tex2_t c_uv;
	uint32_t color;
} face_t; // stores vertex indices for each face/triangle of a mesh

typedef struct {
	vec4_t points[3];
	tex2_t texcoords[3];
	uint32_t color;
	float avg_depth;
} triangle_t; // stores the actual vec2 points of the triangle in the screen

// Note: Pikuma has draw_triangle moved here in lesson "Texture Typedef".
// However I do not recall moving the function here in any of the lessons.
// For the time being I'll keep the draw_triangle function in display.h/.c
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

void draw_texel(
	int x, int y, uint32_t* texture, // the pixel values I want to paint and the texture to pick the color from
	vec4_t point_a, vec4_t point_b, vec4_t point_c, // triangle vertices 
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv // uv coordinates for each triangle vertex
);

// the w's carry the original z values
void draw_textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0, // vertex A
	int x1, int y1, float z1, float w1, float u1, float v1, // vertex B
	int x2, int y2, float z2, float w2, float u2, float v2, // vertex C
	uint32_t* texture
);

#endif
