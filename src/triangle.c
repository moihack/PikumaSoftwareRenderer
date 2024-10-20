#include "display.h"
#include "triangle.h"
#include "swap.h"

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
	// We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
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

///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle based on a texture array of colors.
// We split the original triangle in two, half flat-bottom and half flat-top.
///////////////////////////////////////////////////////////////////////////////
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1		   \
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//
///////////////////////////////////////////////////////////////////////////////
void draw_textured_triangle(
	int x0, int y0, float u0, float v0,
	int x1, int y1, float u1, float v1,
	int x2, int y2, float u2, float v2,
	uint32_t* texture)
{
	// The function body that follows is basically a combination of draw_filled_triangle
	// and fill_flat_top_triangle and fillat_flat_bottom_triangle functions.
	// It's not a 1:1 copy of these functions and does not use the midpoint formula making it more complex.

	// We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
	// Note this time we also "carry" the uv per point (float_swap)
	if (y0 > y1)
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}
	if (y1 > y2)
	{
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&u1, &u2);
		float_swap(&v1, &v2);
	}
	if (y0 > y1) // not a typo since order may have changed in (y1 > y2), so we have to check (y0 > y1) again
	{
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}

	//////////////////////////////////////////////////////
	// Render the upper part of the triangle (flat-bottom)
	//////////////////////////////////////////////////////
	float inv_slope1 = 0;
	float inv_slope2 = 0;
	if ((y1 - y0) != 0) inv_slope1 = (float)(x1 - x0) / abs(y1 - y0); // why abs here?
	if ((y2 - y0) != 0) inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);

	if ((y1 - y0) != 0) // small fix for lines drawing in flat top triangles
	{
		for (int y = y0; y <= y1; y++)
		{
			int x_start = x1 + ((y - y1) * inv_slope1);
			int x_end = x0 + ((y - y0) * inv_slope2);

			// ensure the second for loop will always run, 
			// otherwise some faces will not get rendered
			// loop won't have run to draw_pixels, so they stayed black
			if (x_end < x_start) // this can happen due to transformations applied
			{
				int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
			}

			for (int x = x_start; x < x_end; x++)
			{
				draw_pixel(x, y, (x % 2 == 0 && y % 2 == 0) ? 0xFFFF00FF : 0x00000000);
			}
		}
	}

	//////////////////////////////////////////////////////
	// Render the bottom part of the triangle (flat-top)
	//////////////////////////////////////////////////////
	inv_slope1 = 0;
	inv_slope2 = 0;
	if ((y2 - y1) != 0) inv_slope1 = (float)(x2 - x1) / abs(y2 - y1); // why abs here?
	if ((y2 - y0) != 0) inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);

	if ((y2 - y1) != 0) // small fix for lines drawing in flat top triangles
	{
		for (int y = y1; y <= y2; y++)
		{
			int x_start = x1 + ((y - y1) * inv_slope1);
			int x_end = x0 + ((y - y0) * inv_slope2);

			// ensure the second for loop will always run, 
			// otherwise some faces will not get rendered
			// loop won't have run to draw_pixels, so they stayed black
			if (x_end < x_start) // this can happen due to transformations applied
			{
				int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
			}

			for (int x = x_start; x < x_end; x++)
			{
				draw_pixel(x, y, (x % 2 == 0 && y % 2 == 0) ? 0xFFFF00FF : 0x00000000);
			}
		}
	}
}
