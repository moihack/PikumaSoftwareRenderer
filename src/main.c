#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "matrix.h"
#include "light.h"

// Array of triangles that should be rendered frame by frame
triangle_t* triangles_to_render = NULL;

// Global variables for execution status and game loop
vec3_t camera_position = { .x = 0, .y = 0 , .z = 0 };
mat4_t proj_matrix;

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

	// Initialize the perspective projection matrix
	float fov = 3.141592 / 3.0; // fov value in radians, not degrees - 60 degrees used
	float aspect = (float)window_height / (float)window_width;
	float znear = 0.1;
	float zfar = 100.0;
	proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);
	
	// Loads the cube values in the mesh data structure
	//load_cube_mesh_data();
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

	// Change the mesh scale/rotation values per animation frame
	mesh.rotation.x += 0.01;
	mesh.rotation.y += 0.01;
	mesh.rotation.z += 0.01;
	//mesh.scale.x += 0.002;
	//mesh.scale.y += 0.001;
	//mesh.translation.x += 0.01;
	mesh.translation.z = 5.0;
	
	// Create a scale, rotation and translation matrix that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);
	
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

		vec4_t transformed_vertices[3];

		// Loop all three vertices of this current face/triangle and apply transformations
		for (int j = 0; j < 3; j++)
		{
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

			// Create a World Matrix combining scale, rotation and translation matrices
			// TODO: world_matrix can be created outside the for loop
			// so not all mat_mul functions get calculated each loop
			// however for the sake of closely following the course let's keep it here
			
			// Oder matters: First scale, then rotate, then translate
			// [T]*[R]*[S]*v (expression must be read from right to left)
			mat4_t world_matrix = mat4_identity();
			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
			
			// Multiply the world matrix by the original vector
			transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
			
			// Save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}
		
		// check moved below so we always calculate normal for use in light/shading
		// just kept in place to showcase where it was before that lesson
		// possible cleanup in the future
		//if (cull_method == CULL_BACKFACE) 
		
		// Triangle ACB in clocwise order (CW)
		vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
		vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
		vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

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

		// Backface culling test to see if the current face should be projected
		if (cull_method == CULL_BACKFACE)
		{
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

		vec4_t projected_points[3];

		// Loop all three vertices (of a face/triangle) to perform the projection
		for (int j = 0; j <3; j++) 
		{
			//project the current vertex
			projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

			// Order of transformations still matters, so scale first and translate last
			
			// Scale into the view
			projected_points[j].x *= (window_width / 2.0);
			projected_points[j].y *= (window_height / 2.0);
			
			// Translate the projected points to the middle of the screen
			projected_points[j].x += (window_width / 2.0);
			projected_points[j].y += (window_height / 2.0);
		}

		// Calculate the average depth for each face based on the vertices Z value after transformation
		float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3;
		
		// Calculate the shade intensity based on how aligned is the face normal and the inverse of the light ray
		// Notes: we need the dot with the inverse of the light ray,
		// dot product is just a float so a simple minus (-) symbol can be used
		// Alternatively we could flip light.direction inside light.c 
		// (currently is 1 on .z but we could change it to -1 to achieve the same effect).
		// However Pikuma wanted to keep the light direction the same (going into the screen -> towards the model)
		// so it more correctly alligns with the real world, on how the light ray would behave.
		float light_intensity_factor = -vec3_dot(normal, light.direction);

		// Calculate the triangle color based on light angle
		uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);
		
		triangle_t projected_triangle = {
			.points = {
				{ projected_points[0].x, projected_points[0].y },
				{ projected_points[1].x, projected_points[1].y },
				{ projected_points[2].x, projected_points[2].y },
			},
				.color = triangle_color,
				.avg_depth = avg_depth
		};

		// Save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);
	}

	// Sort the triangles to render by their avg_depth (bubble-sort)
	// if performance becomes an issue we can probably replace the sorting algorithm here
	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++) {
		for (int j = i; j < num_triangles; j++) {
			if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {
				// Swap the triangles positions in the array
				triangle_t temp = triangles_to_render[i];
				triangles_to_render[i] = triangles_to_render[j];
				triangles_to_render[j] = temp; 
			}
		}
	}
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