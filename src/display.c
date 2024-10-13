#include "display.h"
#include <stdio.h> // for stderr

// definition of extern global variables
// declared in display.h but also need to be defined somewhere (https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-error-lnk2001?view=msvc-170)
// otherwise LNK2001 (and finally LNK1120) errors will occur
// also check declarations in display.h for extern keyword use
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL; //pointer to first element in 1D array of 4 byte color values
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800; // int just for code simplicity according to pikuma
int window_height = 600;

enum cull_method cull_method = CULL_BACKFACE; // could also be just enum cull_method cull_method;
enum render_method render_method = RENDER_WIRE; // could also be just enum render_method render_method;

// NOTE from : https://en.wikipedia.org/wiki/Void_type
// The C syntax to declare a (non-variadic) function 
// with an as-yet-unspecified number of parameters, 
// e.g. void f() above, was deprecated in C99.[3] 
// In C23 (and C++), a function prototype with empty parentheses 
// declares a function with zero parameters.[4][5]
bool initialize_window(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	// Use SDL to query what is the max width,height of the display
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width = display_mode.w;
	window_height = display_mode.h;

	// Create an SDL Window
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS
	);

	if (!window)
	{
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	// Create an SDL renderer
	renderer = SDL_CreateRenderer(window, -1, 0);

	if (!renderer)
	{
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}

	// This line enables fullscreen mode, otherwise it would be borderless fullscreen windowed instead
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	return true;
}

void draw_grid(void) // basically a copy of clear_color_buffer with modified step size
{
	for (int y = 0; y < window_height; y += 10)
	{
		for (int x = 0; x < window_width; x += 10)
		{
			//if (x % 10 == 0 || y % 10 == 0) { // for "real grid" - remember to revert step size to y++ and x++
			color_buffer[(window_width * y) + x] = 0xFF333333;
			//}
		}
	}
}

void draw_pixel(int x, int y, uint32_t color)
{
	if (x >=0 && x < window_width && y>=0 && y < window_height) //only if valid index
	{
		color_buffer[(window_width * y) + x] = color;
	}
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

	// one of them is gonna be > 1 and the other one < 1
	float x_inc = delta_x / (float) longest_side_length;
	float y_inc = delta_y / (float) longest_side_length;

	float current_x = x0;
	float current_y = y0;
	for (int i = 0; i <= longest_side_length; i++)
	{
		draw_pixel(round(current_x), round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}

}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}

void draw_rect(int x, int y, int width, int height, uint32_t color) // yet again a similar to the clear color buffer function
{
	// pikuma's solution
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int current_x = x + i;
			int current_y = y + j;
			//color_buffer[(window_width * current_y) + current_x] = color; // Line removed in orthographic projection lesson - still seems worse for performance to draw the draw pixel function instead. TODO: Investigate performance penalty of this change
			draw_pixel(current_x, current_y, color);
		}
	}

	// my solution - practically the same but without local variables inside the for loop
	// also my solution colors the buffer per row while pikuma's per column
	/*for (int h = y; h < y + height; h++) // for each row in rect
	{
		for (int w = x; w < x+width; w++) // for each column per row in rect
		{
			color_buffer[ (window_width*h) + w ] = color; // color row of rect
		}
	} */
}

void render_color_buffer(void)
{
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color)
{
	for (int y = 0; y < window_height; y++)
	{
		for (int x = 0; x < window_width; x++)
		{
			// since color_buffer is 1D array
			// a single for loop could have been used instead
			// for the time being code just follows along with the course  
			color_buffer[(window_width * y) + x] = color;
		}
	}
}

void destroy_window(void)
{
	// destroy things in reverse order of creating them
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}