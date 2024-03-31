#include <stdio.h> // for NULL
#include "array.h"
#include "mesh.h"

// Definition and initialization of 
// extern variables declared in mesh.h

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0}
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
	{.x = -1, .y = -1, .z = -1 }, // 1
	{.x = -1, .y = 1, .z = -1 }, // 2
	{.x = 1, .y = 1, .z = -1 }, // 3
	{.x = 1, .y = -1, .z = -1 }, // 4
	{.x = 1, .y = 1, .z = 1 }, // 5
	{.x = 1, .y = -1, .z = 1 }, // 6
	{.x = -1, .y = 1, .z = 1 }, // 7
	{.x = -1, .y = -1, .z = 1 }  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3 }, // each line here represents a triangle
    {.a = 1, .b = 3, .c = 4 },
    // right
    {.a = 4, .b = 3, .c = 5 },
    {.a = 4, .b = 5, .c = 6 },
    // back
    {.a = 6, .b = 5, .c = 7 },
    {.a = 6, .b = 7, .c = 8 },
    // left
    {.a = 8, .b = 7, .c = 2 },
    {.a = 8, .b = 2, .c = 1 },
    // top
    {.a = 2, .b = 7, .c = 5 },
    {.a = 2, .b = 5, .c = 3 },
    // bottom
    {.a = 6, .b = 8, .c = 1 },
    {.a = 6, .b = 1, .c = 4 }
};

// TODO: Create implementation for mesh.h functions

void load_cube_mesh_data(void)
{
    for (int i = 0; i < N_CUBE_VERTICES; i++)
    {
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }
    for (int i = 0; i < N_CUBE_FACES; i++)
    {
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }

    // The following 2 lines is valid code (compiles and runs normally)
    // However since we are using the array MACROS from array.c
    // the line "int num_faces = array_length(mesh.faces);" in main.c
    // will return a wrong num_faces (correct should be 12 for cube),
    // resulting in out of bounds access in mesh.faces array and a crash.
    // If num_faces is set to 12, the loop runs fine,
    // only for the next crash to occur when quitting the app
    // at array_free(mesh.faces); array_free(mesh.vertices);
    // in free_resources function in main.c.
    // Again the culprit is a hacky MACRO from array.c.
    // Pikuma's solution was using a for loop from the start,
    // but it bothered on why we couldn't achieve the same behavior in 2 lines
    // and decided to investigate further.
    // 
    //mesh.faces = cube_faces;
    //mesh.vertices = cube_vertices;
}

void load_obj_file_data(char* filename)
{
    // TODO:
    // Read the contents of the .obj file
    // and load the vertices and faces in
    // our mesh.vertices and mesh.faces
}