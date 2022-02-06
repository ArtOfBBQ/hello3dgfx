#include "software_renderer.h"
#include "assert.h"
#include "window_size.h"

void renderer_init() {
    box = get_box();
    rotated_box = get_box();
}

void software_render(
    ColoredVertex * next_gpu_workload,
    uint32_t * next_gpu_workload_size)
{
    box->x += 0.0001f;
    box->y -= 0.0001f;
    box->z += 0.03f;
    box->x_angle += 0.02f;
    if (box->x_angle > 6.28) { box->x_angle = 0.0f; }
    box->y_angle += 0.01f;
    if (box->y_angle > 6.28) { box->y_angle = 0.0f; }
    box->z_angle += 0.03f;
    if (box->z_angle > 6.28) { box->z_angle = 0.0f; }
    
    // x-rotate all triangles
    zTriangle x_rotated_triangles[box->triangles_size];
    for (uint32_t i = 0; i < box->triangles_size; i++) {
        x_rotated_triangles[i] =
            x_rotate_triangle(
                box->triangles + i,
                box->x_angle); 
    }

    // z-rotate all triangles
    zTriangle z_rotated_triangles[box->triangles_size];
    for (uint32_t i = 0; i < box->triangles_size; i++) {
        z_rotated_triangles[i] =
            z_rotate_triangle(
                &x_rotated_triangles[i],
                box->z_angle); 
    }
    
    // y-rotate all triangles
    zTriangle triangles_to_draw[box->triangles_size];
    for (uint32_t i = 0; i < box->triangles_size; i++) {
        triangles_to_draw[i] =
            y_rotate_triangle(
                &z_rotated_triangles[i],
                box->y_angle); 
    }
   
    // sort all triangles so the most distant ones can be
    // drawn first 
    z_sort(&triangles_to_draw[0], box->triangles_size);
    
    simd_float2 position = { box->x, box->y };
    
    for (
        uint32_t i = 0;
        i < box->triangles_size;
        i++)
    {
        ColoredVertex triangle_to_draw[3];
        
        float avg_z = get_avg_z(triangles_to_draw + i);
        float dist_modifier = ((far - box->z) / far);
        assert(dist_modifier > 0.0f);
        assert(dist_modifier < 1.0f);
        float brightness =
            (avg_z + (dist_modifier * 5.0f)) / 6.0f;
        
        simd_float4 triangle_color = {
            brightness + 0.6f - (i * 0.04f),
            brightness / 1.2f,
            brightness / 1.5f,
            1.0f};
        
        ztriangle_to_2d(
            /* recipient: */ triangle_to_draw,
            /* input: */ triangles_to_draw + i,
            /* x_offset: */ box->x,
            /* y_offset: */ box->y,
            /* z_offset: */ box->z,
            /* color: */ triangle_color);
        
        draw_triangle(
            /* vertices_recipient: */ next_gpu_workload,
            /* vertex_count_recipient: */ next_gpu_workload_size,
            /* input: */ triangle_to_draw,
            /* position: */ position);
    }
}

void draw_triangle(
    ColoredVertex * vertices_recipient,
    uint32_t * vertex_count_recipient,
    ColoredVertex input[3],
    simd_float2 position)
{
    uint32_t vertex_i = *vertex_count_recipient;
    
    vertices_recipient[vertex_i] = input[0];
    vertices_recipient[vertex_i].XY[0] += position[0];
    vertices_recipient[vertex_i].XY[1] += position[1];
    vertices_recipient[vertex_i].XY[0] *= WINDOW_WIDTH;
    vertices_recipient[vertex_i].XY[1] *= WINDOW_HEIGHT;
    vertex_i++;
    
    vertices_recipient[vertex_i] = input[1];
    vertices_recipient[vertex_i].XY[0] += position[0];
    vertices_recipient[vertex_i].XY[1] += position[1];
    vertices_recipient[vertex_i].XY[0] *= WINDOW_WIDTH;
    vertices_recipient[vertex_i].XY[1] *= WINDOW_HEIGHT;
    vertex_i++;
    
    vertices_recipient[vertex_i] = input[2];
    vertices_recipient[vertex_i].XY[0] += position[0];
    vertices_recipient[vertex_i].XY[1] += position[1];
    vertices_recipient[vertex_i].XY[0] *= WINDOW_WIDTH;
    vertices_recipient[vertex_i].XY[1] *= WINDOW_HEIGHT;
    vertex_i++;
    
    *vertex_count_recipient += 3;
}

void rotate_triangle(
    ColoredVertex to_rotate[3],
    const float angle)
{
    for (uint32_t i = 0; i < 3; i++) {
        to_rotate[i].XY[0] = 
            (cos(angle) * to_rotate[i].XY[0])
                + (sin(angle) * to_rotate[i].XY[1]);
        to_rotate[i].XY[1] =
            (-sin(angle) * to_rotate[i].XY[0])
                + (cos(angle) * to_rotate[i].XY[1]);
    }
}

