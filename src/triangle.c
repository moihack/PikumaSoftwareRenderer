#include "triangle.h"

void int_swap(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	
}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{

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

	// Calculate the new vertex (Mx, My) using triangle similarity
	int My = y1;
	int Mx = ((float)((x2 - x0) * (y1 - y0)) / (float) (y2 - y0)) + x0;

	// Draw flat-bottom triangle
	fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My);

	// Draw flat-top triangle
	fill_flat_top_triangle(x1, y1, Mx, My, x2, y2);
}
