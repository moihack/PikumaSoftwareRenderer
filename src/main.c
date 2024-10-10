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

vec3_t camera_position = { 0, 0 , 0 };

float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

void setup(void)
{
	// Initialize render mode and triangle culling method
	render_method = RENDER_WIRE;
	cull_method = CULL_BACKFACE;

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
	load_cube_mesh_data();
	//load_obj_file_data("./assets/cube.obj");
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
				is_running = false;
			if (event.key.keysym.sym == SDLK_1)
				render_method = RENDER_WIRE_VERTEX;
			if (event.key.keysym.sym == SDLK_2)
				render_method = RENDER_WIRE;
			if (event.key.keysym.sym == SDLK_3)
				render_method = RENDER_FILL_TRIANGLE;
			if (event.key.keysym.sym == SDLK_4)
				render_method = RENDER_FILL_TRIANGLE_WIRE;
			if (event.key.keysym.sym == SDLK_c)
				cull_method = CULL_BACKFACE;
			if (event.key.keysym.sym == SDLK_d)
				cull_method = CULL_NONE;
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
	mesh.rotation.y += 0.01;
	mesh.rotation.z += 0.01;

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

		vec3_t transformed_vertices[3];

		// Loop all three vertices of this current face/triangle and apply transformations
		for (int j = 0; j < 3; j++)
		{
			vec3_t transformed_vertex = face_vertices[j];

			transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

			//Translate the vertex away from the camera - push them inside the monitor (Z grows inside the monitor in the Left-Handed coordinate system)
			transformed_vertex.z += 5;
			
			// Save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}

		// Backface culling test to see if the current face should be projected
		if (cull_method == CULL_BACKFACE)
		{
			// Triangle ACB in clocwise order (CW)
			vec3_t vector_a = transformed_vertices[0]; /*   A   */
			vec3_t vector_b = transformed_vertices[1]; /*  / \  */
			vec3_t vector_c = transformed_vertices[2]; /* C---B */

			vec3_t vector_ab = vec3_sub(vector_b, vector_a);
			vec3_t vector_ac = vec3_sub(vector_c, vector_a);
			vec3_normalize(&vector_ab);
			vec3_normalize(&vector_ac);

			// Compute the face normal (using cross product to find perpendicular)
			// Remember order of arguments matters for cross product direction
			vec3_t normal = vec3_cross(vector_ab, vector_ac);

			// Normalize the face normal vector
			// normalized vector = vector with length 1 (hence .x , .y between 0.0 and 1.0)
			// face normal vector = just a perpendicular vector to a face/triangle surface
			vec3_normalize(&normal);

			// Find the vector between a point in the triangle and the camera origin
			vec3_t camera_ray = vec3_sub(camera_position, vector_a);

			// Calculate how aligned the camera ray is with the face normal using dot product
			float dot_normal_camera = vec3_dot(camera_ray, normal); // order does not matter for dot product, like it does in cross product

			// Bypass the triangles that are looking away from the camera
			if (dot_normal_camera < 0)
			{
				continue;
			}

			// NOTE : Some small observations after messing around with the 
			// if statement above and cross product result.
			// 
			// in case you invert the if above to (dot_normal_camera > 0)
			// then you can "look inside" the meshes, pretty much like when normals
			// are inverted in an engine like Unreal.
			// In that case changing the cross product arguments order, corrects back that issue
			// 
			// So this indeed showcases that order indeed matters for cross product,
			// as it is then used in dot product operation to determine face-camera alignment.
			// 
			// One more way to "invert" the normals is to multiply cross product * -1.0
			// achieving the same effect of "looking inside" the meshes.
			// In that case the if statement above has to be inverted (dot_normal_camera > 0)
			// so that the rendering is correct again.
		}

		vec2_t projected_points[3];

		// Loop all three vertices (of a face/triangle) to perform the projection
		for (int j = 0; j <3; j++) 
		{
			//project the current vertex
			projected_points[j] = project(transformed_vertices[j]);

			// Scale and translate the projected points to the middle of the screen
			projected_points[j].x += (window_width / 2);
			projected_points[j].y += (window_height / 2);
		}

		// Calculate the average depth for each face based on the vertices Z value after transformation
		float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3;

		triangle_t projected_triangle = {
			.points = {
				{ projected_points[0].x, projected_points[0].y },
				{ projected_points[1].x, projected_points[1].y },
				{ projected_points[2].x, projected_points[2].y },
			},
				.color = mesh_face.color,
				.avg_depth = avg_depth
		};

		// Save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);
	}

	// TODO: Sort the triangles to render by their avg_depth
}

void render(void)
{
	draw_grid();

	//Loop all projected triangles and render them
	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++)
	{
		triangle_t triangle = triangles_to_render[i];

		// Draw filled triangle
		if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
		{
			draw_filled_triangle(
				triangle.points[0].x, triangle.points[0].y, // vertex A
				triangle.points[1].x, triangle.points[1].y, // vertex B
				triangle.points[2].x, triangle.points[2].y, // vertex C
				triangle.color
			);
		}

		// Draw triangle wireframe
		if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE)
		{
			draw_triangle(
				triangle.points[0].x, triangle.points[0].y, // vertex A
				triangle.points[1].x, triangle.points[1].y, // vertex B
				triangle.points[2].x, triangle.points[2].y, // vertex C
				0xFFFFFFFF
			);
		}

		// Draw triangle vertex points
		if (render_method == RENDER_WIRE_VERTEX)
		{
			// 6x6 size and -3 in x,y just for centering the vertices in the correct position for viewing
			draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFFFF00); //yellow (ARGB8888)
			draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFFFF00);
			draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFFFF00);
		}
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