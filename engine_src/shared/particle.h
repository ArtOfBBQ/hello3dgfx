#ifndef PARTICLE_H
#define PARTICLE_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PARTICLE_TEXTURES 10
#define PARTICLE_RGBA_PROGRESSION_MAX 10

#include "cpu_to_gpu_types.h"
#include "clientlogic_macro_settings.h"
#include "common.h"
#include "logger.h"
#include "tok_random.h"
#include "zpolygon.h"

#define LINEPARTICLE_EFFECTS_SIZE 100
#define MAX_LINEPARTICLE_DIRECTIONS 5
typedef struct LineParticle {
    zPolygonCPU zpolygon_cpu;
    GPUPolygon zpolygon_gpu;
    GPUPolygonMaterial zpolygon_material;
    
    uint64_t random_seed;
    uint64_t elapsed;
    uint64_t trail_delay;
    uint64_t wait_first;
    
    uint64_t waypoint_duration[MAX_LINEPARTICLE_DIRECTIONS];
    float waypoint_x[MAX_LINEPARTICLE_DIRECTIONS];
    float waypoint_y[MAX_LINEPARTICLE_DIRECTIONS];
    float waypoint_z[MAX_LINEPARTICLE_DIRECTIONS];
    float waypoint_r[MAX_LINEPARTICLE_DIRECTIONS];
    float waypoint_g[MAX_LINEPARTICLE_DIRECTIONS];
    float waypoint_b[MAX_LINEPARTICLE_DIRECTIONS];
    float waypoint_a[MAX_LINEPARTICLE_DIRECTIONS];
    float waypoint_scalefactor[MAX_LINEPARTICLE_DIRECTIONS];
    uint32_t waypoints_size;
    
    uint64_t particle_zangle_variance_pct;
    uint64_t particle_scalefactor_variance_pct;
    uint64_t particle_rgb_variance_pct;
    uint32_t particle_count;
    
    uint32_t deleted;
    uint32_t committed;
} LineParticle;
extern LineParticle * lineparticle_effects;
extern uint32_t lineparticle_effects_size;
LineParticle * next_lineparticle_effect(void);
LineParticle * next_lineparticle_effect_with_zpoly(
    zPolygonCPU * construct_with_zpolygon,
    GPUPolygon * construct_with_polygon_gpu,
    GPUPolygonMaterial * construct_with_polygon_material);
void commit_lineparticle_effect(
    LineParticle * to_commit);
void add_lineparticle_effects_to_workload(
    GPUDataForSingleFrame * frame_data,
    uint64_t elapsed_nanoseconds);

typedef struct ShatterEffect {
    zPolygonCPU zpolygon_to_shatter_cpu;
    GPUPolygon zpolygon_to_shatter_gpu;
    GPUPolygonMaterial zpolygon_to_shatter_material;
    
    uint64_t random_seed;
    uint64_t elapsed;
    uint64_t wait_first;
    
    uint64_t longest_random_delay_before_launch;
    
    uint64_t start_fade_out_at_elapsed;
    uint64_t finish_fade_out_at_elapsed;
    
    float exploding_distance_per_second;
    
    float linear_distance_per_second;
    float linear_direction[3];
    
    float squared_distance_per_second;
    float squared_direction[3];
    
    float xyz_rotation_per_second[3];
    float rgb_bonus_per_second[3];
    
    uint32_t limit_triangles_to; // 0 to render all triangles
    bool32_t deleted;
    bool32_t committed;
} ShatterEffect;
extern ShatterEffect * shatter_effects;
extern uint32_t shatter_effects_size;

ShatterEffect * next_shatter_effect(void);
ShatterEffect * next_shatter_effect_with_zpoly(
    zPolygonCPU * construct_with_zpolygon,
    GPUPolygon * construct_with_polygon_gpu,
    GPUPolygonMaterial * construct_with_polygon_material);
void commit_shatter_effect(
    ShatterEffect * to_commit);

void add_shatter_effects_to_workload(
    GPUDataForSingleFrame * frame_data,
    uint64_t elapsed_nanoseconds);

typedef struct ParticleEffect {
    GPUPolygon gpustats_linear_add;
    GPUPolygon gpustats_linear_variance_multipliers;
    GPUPolygon gpustats_exponential_add;
    
    // Reminder on the way the linear variance multipliers work:
    // -> random floats are generated from 0.0f to 1.0f
    // -> those numbers are multiplied by the variance multipliers
    //    e.g. if you set one to 0.1f, the final new rand will in [0.0f - 0.1f]
    // -> each property adds to itself (rand * self)
    // -> each property subtracts from itself (rand2 * self)
    // You now have a property with some variance introduced. Most will center
    // around the original value and the exceedingly rare case will be
    // (linear_variance_multiplier * self) below or above its original value
    
    zPolygonCPU zpolygon_cpu;
    GPUPolygon zpolygon_gpu;
    GPUPolygonMaterial zpolygon_material;
    
    int32_t object_id;
    
    uint64_t random_seed;
    uint64_t elapsed;
    bool32_t deleted;
    
    uint32_t particle_spawns_per_second;
    
    uint64_t particle_lifespan;
    uint64_t pause_between_spawns;
    
    bool32_t generate_light;
    float light_reach;
    float light_strength;
    float light_rgb[3];
} ParticleEffect;

extern ParticleEffect * particle_effects;
extern uint32_t particle_effects_size;

void construct_particle_effect(ParticleEffect * to_construct);

void request_particle_effect(ParticleEffect * to_request);

void delete_particle_effect(int32_t with_object_id);

void add_particle_effects_to_workload(
    GPUDataForSingleFrame * frame_data,
    uint64_t elapsed_nanoseconds);

#ifdef __cplusplus
}
#endif

#endif // PARTICLE_H
