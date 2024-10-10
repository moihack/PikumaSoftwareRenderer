#include "display.h"
#include "triangle.h"

void int_swap(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	// Find the two slopes (two triangle legs)
	// We need to find the inverse of the slope
	// Normal slope is Dy/Dx (y1 - y0) / (x1 - x0)
	// but inverted slope is Dx/Dy
	float inv_slope1 = (float)(x1 - x0) / (y1 - y0);
	float inv_slope2 = (float)(x2 - x0) / (y2 - y0);

	// Start x_start and x_end from the top vertex (x0,y0)
	float x_start = x0;
	float x_end = x0;

	// Loop all the scanlines from top to bottom - y2 equals y1, so either one works (see comment/figure above signature)
	for (int y = y0; y <= y2; y++)
	{
		// we could also use another for loop from x_start to x_end 
		// and use draw_pixel instead, perhaps leading to faster code
		// as the draw_line does many extra calculations apart from coloring
		// but let's leave it with draw_line for now for cleaner code
		draw_line(x_start, y, x_end, y, color); 
		x_start += inv_slope1;
		x_end += inv_slope2;		
	}
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	// also see comments in fill_flat_bottom_triangle
	// esentially this function is a modified copy of fill_flat_bottom_triangle
	// where the changes are mainly in the for loop y-- instead of y++, -= instead of += etc
	// 
	// Find the two slopes (two triangle legs) - we still need to find the inverse slopes
	float inv_slope1 = (float)(x2 - x0) / (y2 - y0);
	float inv_slope2 = (float)(x2 - x1) / (y2 - y1);

	// Start x_start and x_end from the bottom vertex (x2,y2)
	float x_start = x2;
	float x_end = x2;

	// Loop all the scanlines from bottom to top - y0 equals y1, so either one works (see comment/figure above signature)
	for (int y = y2; y >= y0; y--)
	{
		// we could also use another for loop from x_start to x_end 
		// and use draw_pixel instead, perhaps leading to faster code
		// as the draw_line does many extra calculations apart from coloring
		// but let's leave it with draw_line for now for cleaner code
		draw_line(x_start, y, x_end, y, color);
		x_start -= inv_slope1;
		x_end -= inv_slope2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
///////////////////////////////////////////////////////////////////////////////
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)------(Mx,My)
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	// We need to sort the vertices by y-coordinate ascnding (y0 < y1 < y2)
	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
	}
	if (y1 > y2)
	{
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
	}
	if (y0 > y1) // not a typo since order may have changed in (y1 > y2), so we have to check (y0 > y1) again
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
	}

	// avoid "division by zero" cases inside fill_flat functions
	// these can occur when a triangle is already (after all transformations) a flat-top or flat-bottom one

	if (y1 == y2) // see figure above function signature to visualize the "division by zero" case in your head
	{
		// If true we simply have to draw a flat-bottom triangle
		fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
	}
	else if (y0 == y1) // see figure above function signature to visualize the "division by zero" case in your head
	{
		// If true we simply have to draw a flat-top triangle
		fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
	}
	else // generic case like shown in figure above function signature
	{
		// Calculate the new vertex (Mx, My) using triangle similarity
		int My = y1;
		int Mx = ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;

		// Draw flat-bottom triangle
		fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

		// Draw flat-top triangle
		fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
	}
}
