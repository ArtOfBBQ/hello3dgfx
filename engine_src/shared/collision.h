#ifndef TOK_COLLISION_H
#define TOK_COLLISION_H

#include <math.h> // sqrtf() if no simd

#define COLLISION_SILENCE
#ifndef COLLISION_SILENCE
#include <stdio.h>
#endif

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif

int point_hits_AArect(
    const float point[2],
    const float rect_bounds_min[2],
    const float rect_bounds_max[2]);

float ray_hits_AArect(
    const float ray_origin[2],
    const float ray_direction[2],
    const float rect_bounds_min[2],
    const float rect_bounds_max[2],
    float * collision_recipient);

int point_hits_AAbox(
    const float point[3],
    const float rect_bounds_min[3],
    const float rect_bounds_max[3]);

float ray_hits_AAbox(
    const float ray_origin[3],
    const float ray_direction[3],
    const float box_bounds_min[3],
    const float box_bounds_max[3],
    float * collision_recipient);

float normalized_ray_hits_sphere(
    const float ray_origin[3],
    const float normalized_ray_direction[3],
    const float sphere_origin[3],
    const float sphere_radius,
    float * collision_recipient);

int point_hits_triangle(
    const float point_xy[2],
    const float triangle_vertex_1[2],
    const float triangle_vertex_2[2],
    const float triangle_vertex_3[2]);

#endif // TOK_COLLISION_H
