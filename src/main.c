#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"

// Array of triangles that should be rendered frame by frame
triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };

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

	// Loads the cube values in the mesh data structure
	load_obj_file_data("./assets/f22.obj");
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

	// Initialize the array of triangles to render
	triangles_to_render = NULL;

	mesh.rotation.x += 0.01;
	mesh.rotation.y += 0.00;
	mesh.rotation.z += 0.00;

	// Loop all triangle faces of our mesh
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++)
	{
		face_t mesh_face = mesh.faces[i]; // current mesh face

		vec3_t face_vertices[3]; // store the points/vertices of the current triangle/face - each one of them is a vec3_t

		// to actualy get the vec3_t vertices/points of current mesh face
		// look in the mesh_vertices array using the indices stored in current mesh face

		face_vertices[0] = mesh.vertices[mesh_face.a - 1]; // indexes are stored starting from 1
		face_vertices[1] = mesh.vertices[mesh_face.b - 1]; // in mesh_faces in mesh.c
		face_vertices[2] = mesh.vertices[mesh_face.c - 1]; // hence the -1 part

		triangle_t projected_triangle;

		// Loop all three vertices of this current face/triangle and apply transformations
		for (int j = 0; j < 3; j++)
		{
			vec3_t transformed_vertex = face_vertices[j];

			transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);
			
			//Translate the vertex away from the camera
			transformed_vertex.z -= camera_position.z;

			//project the current point
			vec2_t projected_point = project(transformed_vertex);

			// Scale and translate the projected points to the middle of the screen
			projected_point.x += (window_width / 2);
			projected_point.y += (window_height / 2);

			projected_triangle.points[j] = projected_point;
		}

		// Save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);
	}
}

void render(void)
{
	draw_grid();

	//Loop all projected triangles and render them
	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++)
	{
		triangle_t  triangle = triangles_to_render[i];

		// Draw vertex points
		draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00); //yellow (ARGB8888)
		draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
		draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);
	
		// Draw unfilled triangle
		draw_triangle(
			triangle.points[0].x, 
			triangle.points[0].y,
			triangle.points[1].x, 
			triangle.points[1].y, 
			triangle.points[2].x, 
			triangle.points[2].y,
			0xFF00FF00
			);
	}
	
	// Clear the array of triangles to render every frame loop
	array_free(triangles_to_render);

	render_color_buffer();
	clear_color_buffer(0xFF000000); // black (ARGB8888)

	SDL_RenderPresent(renderer); 
}

// Free the memory that was dynamically allocated by the program
void free_resources(void)
{
	free(color_buffer);
	array_free(mesh.faces);
	array_free(mesh.vertices);
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
	free_resources();

	return 0;
}