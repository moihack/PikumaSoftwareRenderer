#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

// typedef uint32_t color_t; // Pikuma won't be using a typedef for uint32_t color parameters in various functions but we could if we wanted to in the future

enum cull_method
{
	CULL_NONE,
	CULL_BACKFACE
} extern cull_method; // read extern comment below

/* alternatively simply declare the enum
enum cull_method
{
	CULL_NONE,
	CULL_BACKFACE
};
and then write separately (in display.h - this file):
extern enum render_method render_method;
then continue defining in display.c

perhaps this style fits better the current extern variables in display.h/.c
but wanted to showcase that the current implementation is valid syntax as well
*/

enum render_method
{
	RENDER_WIRE,
	RENDER_WIRE_VERTEX,
	RENDER_FILL_TRIANGLE,
	RENDER_FILL_TRIANGLE_WIRE,
	RENDER_TEXTURED,
	RENDER_TEXTURED_WIRE
} extern render_method; // read extern comment below

// only declaration
// The extern keyword means "declare without defining". 
// From : https://stackoverflow.com/a/1433387
// Using extern is only of relevance when the program you're building consists
// of multiple source files linked together, where some of the variables defined,
// for example, in source file file1.c need to be referenced in other source files, such as file2.c
//
// Best way to declare and define global variables
// The clean, reliable way to declare and define global variables 
// is to use a header file to contain an extern declaration of the variable.
// The header is included by the one source file that defines the variable 
// and by all the source files that reference the variable. 
// For each program, one source file (and only one source file) defines the variable. 
// Similarly, one header file (and only one header file) should declare the variable. 
// The header file is crucial; it enables cross-checking between
// independent TUs (translation units - think source files) and ensures consistency.

// Example: in our case global vars are declared as extern in display.h (this file),
// then defined in display.c and can be referenced in main.c (and other .c files) by including display.h
extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern uint32_t* color_buffer; //pointer to first element in 1D array of 4 byte color values
extern float* z_buffer; //pointer to first element in 1D array of float depth values
extern SDL_Texture* color_buffer_texture;

extern int window_width; // int just for code simplicity according to pikuma
extern int window_height; // could also be uint32_t etc

bool initialize_window(void);
void draw_grid(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void clear_z_buffer(void);
void destroy_window(void);

#endif