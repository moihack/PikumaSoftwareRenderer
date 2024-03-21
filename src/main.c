#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "vector.h"

// can't be used as vec3_t cube_points[N_POINTS] 
// the compiler will complain, use define instead
//const int N_POINTS = 9 * 9 * 9; 
#define N_POINTS (9 * 9 * 9)

//Declare an array of vectors/points
vec3_t cube_points[N_POINTS]; // 9x9x9 cube
vec2_t projected_points[N_POINTS];

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };

float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

void setup(void)
{
	// allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
	
	// Creating an SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	int point_count = 0;

	// Start loading my array of vectors
	// From -1 to 1 (in this 9x9x9 cube)
	for (float x = -1; x <= 1; x += 0.25)
	{
		for (float y = -1; y <= 1; y += 0.25)
		{
			for (float z = -1; z <= 1; z += 0.25)
			{
				vec3_t new_point = { .x = x, .y = y, .z = z }; //vec3_t new_point = {x,y,z} // would be also valid , .x refers to struct's x variable
				cube_points[point_count++] = new_point;
			}
		}
	}
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

//Function that receives a 3D vector and returns a project 2D point
vec2_t project(vec3_t point)
{
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};

	return projected_point;
}

void update(void)
{
	// old way of waiting for specific time consumed more CPU
	// just kept in place for comparison with new way (SDL_Delay)
	// while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));

	// Comment by Darko Draskovic:
	// (SDL_GetTicks() - previous_frame_time) will always be positive, 
	// so the range of time_to_wait is (-infinity, FRAME_TARGET_TIME]
	// FRAME_TARGET_TIME, is the case if the update took 0 seconds.
	// Hence the check "time_to_wait <= FRAME_TARGET_TIME"
	// inside the if statement below is redundant.
	// 
	// However according to Pikuma's reply:
	// Most applications usually have a capped maximum value of delta_time. 
	// One of the reasons for this is if we try to debug our program. 
	// Pausing the execution line by line, we don't want the delta_time 
	// (time it took from the previous frame to the next) 
	// to be huge and mess up our animation, 
	// making our object jump several pixels for example.
	// But concludes agreeing that: 
	// "in this case the condition checking if time_to_wait
	//  is less than FRAME_TARGET_TIME is mostly useless"
	
	// Wait some time until we reach the target frame time in milliseconds
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
	
	// Only delay execution if we are running too fast
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}
	previous_frame_time = SDL_GetTicks();

	cube_rotation.x += 0.01;
	cube_rotation.y += 0.01; //increase y rotation each frame
	cube_rotation.z += 0.01;

	for (int i = 0; i < N_POINTS; i++)
	{
		vec3_t point = cube_points[i];

		vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
		transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
		transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

		//Move the points away from the camera
		transformed_point.z -= camera_position.z;

		//project the current point
		vec2_t projected_point = project(transformed_point);

		//save the projected 2D Vector in the array of projected points
		projected_points[i] = projected_point;
	}
}

void render(void)
{
	draw_grid();

	//Loop all projected points and render them
	for (int i = 0; i < N_POINTS; i++)
	{
		vec2_t projected_point = projected_points[i];
		draw_rect(
			projected_point.x + (window_width / 2),
			projected_point.y + (window_height / 2),
			4,
			4,
			0xFFFFFF00 //yellow (ARGB8888)
		);
	}

	render_color_buffer();
	clear_color_buffer(0xFF000000); // black (ARGB8888)

	SDL_RenderPresent(renderer); 
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