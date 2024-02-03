#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL; //pointer to first element in 1D array of 4 byte color values

int window_width = 800; // int just for code simplicity according to pikuma
int window_height = 600; // could also be uint32_t etc

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

	return true;
}

void setup(void)
{
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
}

void process_input(void)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
		case SDL_QUIT: // NOTE : SDL_QUIT --> [X] close button was pressed
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				is_running = false;
			}
			break;
	}

}

void update(void)
{
	// TODO:
}

void render(void)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_RenderPresent(renderer); 
}

void destroy_window(void)
{
	// destroy things in reverse order of creating them
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	is_running = initialize_window();

	setup();

	while(is_running)
	{
		process_input();
		update();
		render();
	}

	destroy_window();

	return 0;
}