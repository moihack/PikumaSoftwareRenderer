#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

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
// independent TUs (translation units — think source files) and ensures consistency.

// Example: in our case global vars are declared as extern in display.h (this file),
// then defined in display.c and can be referenced in main.c (and other .c files) by including display.h
extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern uint32_t* color_buffer; //pointer to first element in 1D array of 4 byte color values
extern SDL_Texture* color_buffer_texture;

extern int window_width; // int just for code simplicity according to pikuma
extern int window_height; // could also be uint32_t etc

bool initialize_window(void);
void draw_grid(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void destroy_window(void);

#endif
