#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct {
    vec3_t position;
    vec3_t direction; // the direction where the camera is looking at
    vec3_t forward_velocity;
    float yaw; // angle in radians
} camera_t;

extern camera_t camera;

#endif
