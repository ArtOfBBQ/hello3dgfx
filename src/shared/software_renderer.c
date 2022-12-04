#include "software_renderer.h"

static uint32_t renderer_initialized = false;

// all of these are for vectorized
// manipulation of our triangles vertices
#define VERTICES_CAP 500000
static float * polygons_x; // the xyz of the parent polygons
static float * polygons_y;
static float * polygons_z;
static float * triangle_vertices_x; // the xyz offsets of the triangles that make up the poly
static float * triangle_vertices_y;
static float * triangle_vertices_z;
static float * working_memory_1;
static float * working_memory_2;
static float * cosines;
static float * sines;
static float * visibility_ratings;
static Vertex * rendered_vertices;

void init_renderer() {
    renderer_initialized = true;
    
    camera.x = 0.0f;
    camera.y = 0.0f;
    camera.z = 0.0f;
    camera.x_angle = 0.5f;
    camera.y_angle = 0.3f;
    camera.z_angle = 0.2f;
    
    triangle_vertices_x = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    triangle_vertices_y = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    triangle_vertices_z = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    polygons_x = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    polygons_y = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    polygons_z = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    working_memory_1 = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    working_memory_2 = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    cosines = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    sines = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    visibility_ratings = (float *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(float), 32);
    rendered_vertices = (Vertex *)malloc_from_unmanaged_aligned(
        VERTICES_CAP * sizeof(Vertex), 32);
}

void software_render(
    Vertex * next_gpu_workload,
    uint32_t * next_workload_size,
    uint64_t elapsed_nanoseconds)
{
    (void)elapsed_nanoseconds;
    
    if (renderer_initialized != true) {
        log_append("renderer not initialized, aborting...\n");
        return;
    }
    
    if (
        next_gpu_workload == NULL ||
        next_workload_size == NULL)
    {
        log_append("ERROR: platform layer didnt pass recipients\n");
        return;
    }
    
    if (zpolygons_to_render_size == 0) {
        return;
    }
    
    assert(zpolygons_to_render_size < ZPOLYGONS_TO_RENDER_ARRAYSIZE);
    
    uint32_t all_triangles_size = 0;
    for (
        uint32_t i = 0;
        i < zpolygons_to_render_size;
        i++)
    {
        for (
            uint32_t j = 0;
            j < zpolygons_to_render[i].triangles_size;
            j++)
        {
            all_triangles_size++;
        }
    }
    
    if (all_triangles_size == 0) { return; }
    
    // transform all triangle vertices
    // there's actually only 1 angle per polygon, so
    // this is very wasteful
    float x_angles[all_triangles_size * 3];
    float y_angles[all_triangles_size * 3];
    float z_angles[all_triangles_size * 3];
    
    const uint32_t vertices_size = all_triangles_size * 3;
    assert(vertices_size < VERTICES_CAP);
    
    uint32_t cur_vertex = 0;
    for (
        uint32_t i = 0;
        i < zpolygons_to_render_size;
        i++)
    {
        for (
            uint32_t j = 0;
            j < zpolygons_to_render[i].triangles_size;
            j++)
        {
            for (uint32_t m = 0; m < 3; m++) {
                triangle_vertices_x[cur_vertex] =
                    zpolygons_to_render[i].triangles[j].vertices[m].x;
                triangle_vertices_y[cur_vertex] =
                    zpolygons_to_render[i].triangles[j].vertices[m].y;
                triangle_vertices_z[cur_vertex] =
                    zpolygons_to_render[i].triangles[j].vertices[m].z;
                polygons_x[cur_vertex] = zpolygons_to_render[i].x;
                polygons_y[cur_vertex] = zpolygons_to_render[i].y;
                polygons_z[cur_vertex] = zpolygons_to_render[i].z;
                x_angles[cur_vertex] = zpolygons_to_render[i].x_angle;
                y_angles[cur_vertex] = zpolygons_to_render[i].y_angle;
                z_angles[cur_vertex] = zpolygons_to_render[i].z_angle;
                cur_vertex += 1;
            }
        }
    }
        
    for (uint32_t i = 0; i < vertices_size; i++) {
        cosines[i] = cosf(x_angles[i]);
        sines[i] = sinf(x_angles[i]);
    }
    x_rotate_zvertices_inplace(
        triangle_vertices_y,
        triangle_vertices_z, 
        working_memory_1,
        working_memory_2, 
        vertices_size, 
        cosines,
        sines);
    for (uint32_t i = 0; i < vertices_size; i++) {
        cosines[i] = cosf(y_angles[i]);
        sines[i] = sinf(y_angles[i]);
    }
    y_rotate_zvertices_inplace(
        triangle_vertices_x,
        triangle_vertices_z,
        working_memory_1,
        working_memory_2, 
        vertices_size,
        cosines,
        sines);
    for (uint32_t i = 0; i < vertices_size; i++) {
        cosines[i] = cosf(z_angles[i]);
        sines[i] = sinf(z_angles[i]);
    }
    z_rotate_zvertices_inplace(
        triangle_vertices_x,
        triangle_vertices_y,
        working_memory_1,
        working_memory_2, 
        vertices_size,
        cosines,
        sines);
    // translate the world so that the camera becomes 0,0,0
    platform_256_add(triangle_vertices_x, polygons_x, vertices_size);
    platform_256_add(triangle_vertices_y, polygons_y, vertices_size);
    platform_256_add(triangle_vertices_z, polygons_z, vertices_size);
    platform_256_sub_scalar(triangle_vertices_x, vertices_size, camera.x);
    platform_256_sub_scalar(triangle_vertices_y, vertices_size, camera.y);
    platform_256_sub_scalar(triangle_vertices_z, vertices_size, camera.z);
    
    // next: camera-rotate x, y and z
    x_rotate_zvertices_inplace_scalar_angle(
        triangle_vertices_y,
        triangle_vertices_z, 
        working_memory_1,
        working_memory_2, 
        vertices_size, 
        -camera.x_angle);
    y_rotate_zvertices_inplace_scalar_angle(
        triangle_vertices_x,
        triangle_vertices_z,
        working_memory_1,
        working_memory_2, 
        vertices_size, 
        -camera.y_angle);
    z_rotate_zvertices_inplace_scalar_angle(
        triangle_vertices_x,
        triangle_vertices_y,
        working_memory_1,
        working_memory_2, 
        vertices_size, 
        -camera.z_angle);
    
    uint32_t visible_triangles_size = 0;
    
    // we're not using the camera because the entire world
    // was translated to have the camera be at 0,0,0
    zVertex origin;
    origin.x = 0.0f;
    origin.y = 0.0f;
    origin.z = 0.0f;
    
    // this gets 1 'visibility rating' (a dot product) per triangle,
    // so it will be 1/3rd the size of vertices_size    
    get_visibility_ratings(
        origin,
        triangle_vertices_x,
        triangle_vertices_y,
        triangle_vertices_z,
        vertices_size,
        visibility_ratings);
    
    // Next, we'll do a bunch of copying of the visible triangles to the front
    // of the arrays, so we can ignore the invisible triangles at the end
    uint32_t triangle_i = 0;
    for (
        uint32_t zp_i = 0;
        zp_i < zpolygons_to_render_size;
        zp_i++)
    {
        for (
           int32_t i = 0;
           i < zpolygons_to_render[zp_i].triangles_size;
           i++)
        {
            uint32_t first_vertex_i = triangle_i * 3;
            log_assert(first_vertex_i < vertices_size);
            if (
                visibility_ratings[triangle_i] < 0.0f &&
                (triangle_vertices_z[triangle_i * 3] > projection_constants.near ||
                 triangle_vertices_z[(triangle_i * 3)+1] > projection_constants.near ||
                 triangle_vertices_z[(triangle_i * 3)+2] > projection_constants.near))
            {
                for (uint32_t m = 0; m < 3; m++) {
                    uint32_t all_vertices_i = (triangle_i * 3)+m;
                    uint32_t visible_vertices_i = (visible_triangles_size * 3)+m;
                    
                    triangle_vertices_x[(visible_triangles_size * 3)+m] =
                        triangle_vertices_x[all_vertices_i];
                    triangle_vertices_y[(visible_triangles_size * 3)+m] =
                        triangle_vertices_y[all_vertices_i];
                    triangle_vertices_z[(visible_triangles_size * 3)+m] =
                        triangle_vertices_z[all_vertices_i];
                    
                    rendered_vertices[visible_vertices_i].lighting[0] = 0.0f;
                    rendered_vertices[visible_vertices_i].lighting[1] = 0.0f;
                    rendered_vertices[visible_vertices_i].lighting[2] = 0.0f;
                    rendered_vertices[visible_vertices_i].lighting[3] = 1.0f;
                    
                    rendered_vertices[visible_vertices_i].texture_i =
                        zpolygons_to_render[zp_i].triangles[i].texture_i;
                    rendered_vertices[visible_vertices_i].texturearray_i =
                        zpolygons_to_render[zp_i].triangles[i].texturearray_i;
                    
                    rendered_vertices[visible_vertices_i].uv[0] =
                        zpolygons_to_render[zp_i].triangles[i].vertices[m].uv[0];
                    rendered_vertices[visible_vertices_i].uv[1] =
                        zpolygons_to_render[zp_i].triangles[i].vertices[m].uv[1];
                    
                    rendered_vertices[visible_vertices_i].RGBA[0] =
                        zpolygons_to_render[zp_i].triangles[i].color[0];
                    rendered_vertices[visible_vertices_i].RGBA[1] =
                        zpolygons_to_render[zp_i].triangles[i].color[1];
                    rendered_vertices[visible_vertices_i].RGBA[2] =
                        zpolygons_to_render[zp_i].triangles[i].color[2];
                    rendered_vertices[visible_vertices_i].RGBA[3] =
                        zpolygons_to_render[zp_i].triangles[i].color[3];
                }
                visible_triangles_size += 1;
            }
            triangle_i += 1;
        }
    }
    uint32_t visible_vertices_size = visible_triangles_size * 3;
    
    for (
        uint32_t light_i = 0;
        light_i < zlights_to_apply_size;
        light_i++)
    {
        ztriangles_apply_lighting(
            triangle_vertices_x,
            triangle_vertices_y,
            triangle_vertices_z,
            visible_vertices_size,
            rendered_vertices,
            visible_vertices_size,
            &zlights_to_apply[light_i]);
    }
    
    // w is basically just z before projection
    for (uint32_t i = 0; i < visible_triangles_size; i++) {
        rendered_vertices[(i*3)+0].w = triangle_vertices_z[(i*3)+0];
        rendered_vertices[(i*3)+1].w = triangle_vertices_z[(i*3)+1];
        rendered_vertices[(i*3)+2].w = triangle_vertices_z[(i*3)+2]; 
    }
    
    ztriangles_to_2d_inplace(
        triangle_vertices_x,
        triangle_vertices_y,
        triangle_vertices_z,
        visible_vertices_size);
    
    for (uint32_t i = 0; i < visible_triangles_size; i++) {
        // note: this won't overwrite the lighting properties in rendered_vertices,
        // only the positions        
        rendered_vertices[(i*3)+0].x = triangle_vertices_x[(i*3)+0];
        rendered_vertices[(i*3)+1].x = triangle_vertices_x[(i*3)+1];
        rendered_vertices[(i*3)+2].x = triangle_vertices_x[(i*3)+2];        
        rendered_vertices[(i*3)+0].y = triangle_vertices_y[(i*3)+0];
        rendered_vertices[(i*3)+1].y = triangle_vertices_y[(i*3)+1];
        rendered_vertices[(i*3)+2].y = triangle_vertices_y[(i*3)+2]; 
        rendered_vertices[(i*3)+0].z = triangle_vertices_z[(i*3)+0];
        rendered_vertices[(i*3)+1].z = triangle_vertices_z[(i*3)+1];
        rendered_vertices[(i*3)+2].z = triangle_vertices_z[(i*3)+2]; 
        
        draw_triangle(
            /* vertices_recipient: */
                next_gpu_workload,
            /* vertex_count_recipient: */
                next_workload_size,
            /* input: */
                rendered_vertices + (i * 3));
    }
    
    if (!application_running) { return; }
}

