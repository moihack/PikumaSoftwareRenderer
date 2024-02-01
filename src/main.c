#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

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

	// Create an SDL Window
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		600,
		SDL_WINDOW_BORDERLESS
	);

	if (!window)
	{
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	
	if (!renderer)
	{
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	is_running = initialize_window();

	return 0;
}