#ifndef ZPOLYGON_H
#define ZPOLYGON_H

#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <assert.h>

#include "vertex_types.h"
#include "bool_types.h"
#include "window_size.h"
#include "platform_layer.h"
#include "static_redefinitions.h"

// projection constants
typedef struct ProjectionConstants {
    float near;
    float far;
    float z_normalisation;
    float field_of_view;
    float field_of_view_angle;
    float field_of_view_rad;
    float field_of_view_modifier;
    float aspect_ratio;
} ProjectionConstants;

extern ProjectionConstants projection_constants;

void init_projection_constants(void);

typedef struct zVertex {
    float x;
    float y;
    float z;
} zVertex;

typedef struct zTriangle {
    zVertex vertices[3];
} zTriangle;

typedef struct zPolygon {
    zTriangle * triangles;
    uint32_t triangles_size;
    float x;
    float y;
    float z;
    float x_angle;
    float y_angle;
    float z_angle;
} zPolygon;

zPolygon * get_box(void);

void ztriangle_to_2d(
    ColoredVertex recipient[3],
    zTriangle * input,
    float color[4]);

zTriangle x_rotate_triangle(
    const zTriangle * input, const float angle);
zTriangle y_rotate_triangle(
    const zTriangle * input, const float angle);
zTriangle z_rotate_triangle(
    const zTriangle * input, const float angle);

zTriangle translate_ztriangle(
    const zTriangle * input,
    const float by_x,
    const float by_y,
    const float by_z);

zPolygon * load_from_obj_file(char * filename);

void free_zpolygon(
    zPolygon * to_free);

float get_avg_z(
    const zTriangle * of_triangle);
int sorter_cmpr_lowest_z(
    const void * a,
    const void * b);

void z_sort(
    zTriangle * triangles,
    const uint32_t triangles_size);

#endif

