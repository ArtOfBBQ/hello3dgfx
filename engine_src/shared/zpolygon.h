#ifndef ZPOLYGON_H
#define ZPOLYGON_H

#include <math.h>
#include <inttypes.h>
#include <string.h>

#include "clientlogic_macro_settings.h"

#include "simd.h"
#include "logger.h"
#include "common.h"
#include "platform_layer.h"
#include "cpu_gpu_shared_types.h"
#include "lightsource.h"
#include "window_size.h"
#include "texture_array.h"
#include "objmodel.h"

#define FPS_COUNTER_OBJECT_ID 0

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VertexMaterial {
    float color[4];
    int32_t texturearray_i; /*
                            the index in the global var
                            'texture_arrays' of the texturearray
                             -1 for "untextured, use color
                            instead"
                            */
    int32_t texture_i;     // index in texturearray
} VertexMaterial;

typedef struct zPolygonCPU {
    int32_t mesh_id; // data in all_mesh_summaries[mesh_id]
    
    int32_t object_id;
    int32_t touchable_id;
    float hitbox_leftbottomfront[3];
    float hitbox_righttopback[3];
    
    bool32_t alpha_blending_enabled;
    bool32_t committed;
    bool32_t deleted;
    bool32_t visible;
} zPolygonCPU;

void set_zpolygon_hitbox(
    zPolygonCPU * mesh_cpu, GPUPolygon * mesh_gpu);

typedef struct zPolygonCollection {
    GPUPolygon gpu_data[MAX_POLYGONS_PER_BUFFER];
    GPUPolygonMaterial gpu_materials[
        MAX_POLYGONS_PER_BUFFER * MAX_MATERIALS_PER_POLYGON];
    zPolygonCPU cpu_data[MAX_POLYGONS_PER_BUFFER];
    uint32_t size;
} zPolygonCollection;

typedef struct PolygonRequest {
    GPUPolygon * gpu_data;
    GPUPolygonMaterial * gpu_materials;
    zPolygonCPU * cpu_data;
    uint32_t materials_size;
} PolygonRequest;

void construct_zpolygon(
    PolygonRequest * to_construct);
// Allocate a PolygonRequest on the stack, then call this
void request_next_zpolygon(PolygonRequest * stack_recipient);
void commit_zpolygon_to_render(PolygonRequest * to_commit);

// A buffer of zPolygon objects that should be rendered
// in your application
// index 0 to zpolygons_to_render_size will be rendered,
// the rest of the array will be ignored
extern zPolygonCollection * zpolygons_to_render;

void delete_zpolygon_object(const int32_t with_object_id);

float dot_of_vertices_f3(
    const float a[3],
    const float b[3]);

float get_y_multiplier_for_height(
    zPolygonCPU * for_poly,
    const float for_height);
float get_x_multiplier_for_width(
    zPolygonCPU * for_poly,
    const float for_width);

void scale_zpolygon_multipliers_to_height(
    zPolygonCPU * cpu_data,
    GPUPolygon * gpu_data,
    const float new_height);

float get_distance_f3(
    const float p1[3],
    const float p2[3]);

bool32_t ray_intersects_zpolygon_hitbox(
    const float ray_origin[3],
    const float ray_direction[3],
    const zPolygonCPU * cpu_data,
    const GPUPolygon  * gpu_data,
    float * recipient_hit_point);

void construct_quad_around(
    const float mid_x,
    const float mid_y,
    const float z,
    const float width,
    const float height,
    PolygonRequest * stack_recipient);

void construct_quad(
    const float left_x,
    const float bottom_y,
    const float z,
    const float width,
    const float height,
    PolygonRequest * stack_recipient);

void construct_cube_around(
    const float mid_x,
    const float mid_y,
    const float z,
    const float width,
    const float height,
    const float depth,
    PolygonRequest * stack_recipient);

#ifdef __cplusplus
}
#endif

#endif // ZPOLYGON_H
