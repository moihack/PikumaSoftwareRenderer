#ifndef  MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

// Pikuma's comment on extern keyword:
// Here we are declaring these variables
// but they are gonna be defined (later) EXTERNally
// 
// Also see display.h/.c for some previous extern keyword notes

#define N_MESH_VERTICES 8
extern vec3_t mesh_vertices[N_MESH_VERTICES];

#define N_MESH_FACES (6 * 2) // 6 cube faces, 2 triangles per face
extern face_t mesh_faces[N_MESH_FACES];

#endif
