#include "renderer.h"

static uint32_t renderer_initialized = false;

void init_renderer(void) {
    renderer_initialized = true;
    
    camera.xyz[0]       = 0.0f;
    camera.xyz[1]       = 0.0f;
    camera.xyz[2]       = 0.0f;
    camera.xyz_angle[0] = 0.0f;
    camera.xyz_angle[1] = 0.0f;
    camera.xyz_angle[2] = 0.0f;
}

static bool32_t is_last_clicked = false;

static void add_line_vertex(
    GPUDataForSingleFrame * frame_data,
    const float xyz[3],
    const float ignore_camera)
{
    log_assert(frame_data->line_vertices != NULL);
    
    if (frame_data->line_vertices_size >= MAX_LINE_VERTICES) {
        return;
    }
    
    memcpy(
        &frame_data->line_vertices[frame_data->line_vertices_size].xyz,
        xyz,
        sizeof(float) * 3);
    
    frame_data->line_vertices[frame_data->line_vertices_size].ignore_camera =
        ignore_camera;
    
    frame_data->line_vertices_size += 1;
}

static void add_point_vertex(
    GPUDataForSingleFrame * frame_data,
    const float xyz[3],
    const float ignore_camera)
{
    log_assert(frame_data->point_vertices != NULL);
    
    if (frame_data->point_vertices_size >= MAX_POINT_VERTICES) {
        return;
    }
    
    memcpy(
        &frame_data->point_vertices[frame_data->point_vertices_size].xyz,
        xyz,
        sizeof(float) * 3);
    
    frame_data->point_vertices[frame_data->point_vertices_size].ignore_camera =
        ignore_camera;
    
    frame_data->point_vertices_size += 1;
}

inline static void draw_hitbox(
    GPUDataForSingleFrame * frame_data,
    const float xyz[3],
    const float xyz_offset[3],
    const float hitbox_leftbottomfront[3],
    const float hitbox_righttopback[3],
    const float xyz_angle[3],
    const float ignore_camera)
{
    add_point_vertex(frame_data, xyz, ignore_camera);
    
    float leftbottomfront[3];
    memcpy(leftbottomfront, hitbox_leftbottomfront, sizeof(float) * 3);
    
    float righttopback[3];
    memcpy(righttopback, hitbox_righttopback, sizeof(float) * 3);
    
    // TODO: activate rotation & offsets
    //    x_rotate_zvertex_f3(leftbottomfront, -xyz_angle[0]);
    //    y_rotate_zvertex_f3(leftbottomfront, -xyz_angle[1]);
    //    z_rotate_zvertex_f3(leftbottomfront, -xyz_angle[2]);
    //
    //    x_rotate_zvertex_f3(righttopback, -xyz_angle[0]);
    //    y_rotate_zvertex_f3(righttopback, -xyz_angle[1]);
    //    z_rotate_zvertex_f3(righttopback, -xyz_angle[2]);
    //
    //    // TODO: test if offsets work
    //    leftbottomfront[0] += xyz_offset[0];
    //    leftbottomfront[1] += xyz_offset[1];
    //    leftbottomfront[2] += xyz_offset[2];
    
    // left top front -> right top front
    float linestart_xyz[3];
    float lineend_xyz[3];
    linestart_xyz[0] = leftbottomfront[0] + xyz[0];
    linestart_xyz[1] = righttopback[1] + xyz[1];
    linestart_xyz[2] = leftbottomfront[2] + xyz[2];
    lineend_xyz[0] = righttopback[0] + xyz[0];
    lineend_xyz[1] = righttopback[1] + xyz[1];
    lineend_xyz[2] = leftbottomfront[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // left bottom front -> right bottom front
    linestart_xyz[0] = leftbottomfront[0] + xyz[0];
    linestart_xyz[1] = leftbottomfront[1] + xyz[1];
    linestart_xyz[2] = leftbottomfront[2] + xyz[2];
    lineend_xyz[0] = righttopback[0] + xyz[0];
    lineend_xyz[1] = leftbottomfront[1] + xyz[1];
    lineend_xyz[2] = leftbottomfront[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // left top back -> right top back
    linestart_xyz[0] = leftbottomfront[0] + xyz[0];
    linestart_xyz[1] = righttopback[1] + xyz[1];
    linestart_xyz[2] = righttopback[2] + xyz[2];
    lineend_xyz[0] = righttopback[0] + xyz[0];
    lineend_xyz[1] = righttopback[1] + xyz[1];
    lineend_xyz[2] = righttopback[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // left bottom back -> right bottom back
    linestart_xyz[0] = leftbottomfront[0] + xyz[0];
    linestart_xyz[1] = leftbottomfront[1] + xyz[1];
    linestart_xyz[2] = righttopback[2] + xyz[2];
    lineend_xyz[0] = righttopback[0] + xyz[0];
    lineend_xyz[1] = leftbottomfront[1] + xyz[1];
    lineend_xyz[2] = righttopback[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // left top front -> left top back
    linestart_xyz[0] = leftbottomfront[0] + xyz[0];
    linestart_xyz[1] = righttopback[1] + xyz[1];
    linestart_xyz[2] = leftbottomfront[2] + xyz[2];
    lineend_xyz[0] = leftbottomfront[0] + xyz[0];
    lineend_xyz[1] = righttopback[1] + xyz[1];
    lineend_xyz[2] = righttopback[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // right top front -> right top back
    linestart_xyz[0] = righttopback[0] + xyz[0];
    linestart_xyz[1] = righttopback[1] + xyz[1];
    linestart_xyz[2] = leftbottomfront[2] + xyz[2];
    lineend_xyz[0] = righttopback[0] + xyz[0];
    lineend_xyz[1] = righttopback[1] + xyz[1];
    lineend_xyz[2] = righttopback[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // left bottom front -> left bottom back
    linestart_xyz[0] = leftbottomfront[0] + xyz[0];
    linestart_xyz[1] = leftbottomfront[1] + xyz[1];
    linestart_xyz[2] = leftbottomfront[2] + xyz[2];
    lineend_xyz[0] = leftbottomfront[0] + xyz[0];
    lineend_xyz[1] = leftbottomfront[1] + xyz[1];
    lineend_xyz[2] = righttopback[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // right bottom front -> right bottom back
    linestart_xyz[0] = righttopback[0] + xyz[0];
    linestart_xyz[1] = leftbottomfront[1] + xyz[1];
    linestart_xyz[2] = leftbottomfront[2] + xyz[2];
    lineend_xyz[0] = righttopback[0] + xyz[0];
    lineend_xyz[1] = leftbottomfront[1] + xyz[1];
    lineend_xyz[2] = righttopback[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // left top front -> left bottom front
    linestart_xyz[0] = righttopback[0] + xyz[0];
    linestart_xyz[1] = leftbottomfront[1] + xyz[1];
    linestart_xyz[2] = leftbottomfront[2] + xyz[2];
    lineend_xyz[0] = righttopback[0] + xyz[0];
    lineend_xyz[1] = leftbottomfront[1] + xyz[1];
    lineend_xyz[2] = righttopback[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // right top front -> right bottom front
    linestart_xyz[0] = righttopback[0] + xyz[0];
    linestart_xyz[1] = righttopback[1] + xyz[1];
    linestart_xyz[2] = leftbottomfront[2] + xyz[2];
    lineend_xyz[0] = righttopback[0] + xyz[0];
    lineend_xyz[1] = leftbottomfront[1] + xyz[1];
    lineend_xyz[2] = leftbottomfront[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // left top back -> left bottom back
    linestart_xyz[0] = leftbottomfront[0] + xyz[0];
    linestart_xyz[1] = righttopback[1] + xyz[1];
    linestart_xyz[2] = righttopback[2] + xyz[2];
    lineend_xyz[0] = leftbottomfront[0] + xyz[0];
    lineend_xyz[1] = leftbottomfront[1] + xyz[1];
    lineend_xyz[2] = righttopback[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
    
    // right top back -> right bottom back
    linestart_xyz[0] = righttopback[0] + xyz[0];
    linestart_xyz[1] = righttopback[1] + xyz[1];
    linestart_xyz[2] = righttopback[2] + xyz[2];
    lineend_xyz[0] = righttopback[0] + xyz[0];
    lineend_xyz[1] = leftbottomfront[1] + xyz[1];
    lineend_xyz[2] = righttopback[2] + xyz[2];
    add_line_vertex(
        frame_data,
        linestart_xyz,
        ignore_camera);
    add_line_vertex(
        frame_data,
        lineend_xyz,
        ignore_camera);
}

inline static void zpolygon_hitboxes_to_lines(
    GPUDataForSingleFrame * frame_data)
{
    #ifndef LOGGER_IGNORE_ASSERTS
    if (
        window_globals->visual_debug_last_clicked_touchable_id < 0 &&
        !window_globals->visual_debug_mode)
    {
        return;
    }
    
    for (uint32_t zp_i = 0; zp_i < zpolygons_to_render->size; zp_i++) {
        if (zpolygons_to_render->cpu_data[zp_i].touchable_id >= 0) {
            is_last_clicked =
                window_globals->visual_debug_last_clicked_touchable_id ==
                    zpolygons_to_render->cpu_data[zp_i].touchable_id;
            if (window_globals->visual_debug_mode ||
                is_last_clicked)
            {
                draw_hitbox(
                    /* GPUDataForSingleFrame * frame_data: */
                        frame_data,
                    /* const float xyz[3]: */
                        zpolygons_to_render->gpu_data[zp_i].xyz,
                    /* const float xyz_offset[3]: */
                        zpolygons_to_render->gpu_data[zp_i].xyz_offset,
                    /* const float *hitbox_leftbottomfront: */
                        zpolygons_to_render->cpu_data[zp_i].hitbox_leftbottomfront,
                    /* const float *hitbox_righttopback: */
                        zpolygons_to_render->cpu_data[zp_i].hitbox_righttopback,
                    /* const float *xyz_angle: */
                        zpolygons_to_render->gpu_data[zp_i].xyz_angle,
                    /* const float ignore_camera: */
                        zpolygons_to_render->gpu_data[zp_i].ignore_camera);
               }
        }
    }
    #else
    (void)frame_data;
    #endif
}

inline static void add_alphablending_zpolygons_to_workload(
    GPUDataForSingleFrame * frame_data)
{
    frame_data->first_alphablend_i = frame_data->vertices_size;
    
    // Copy all vertices that do use alpha blending
    for (
        int32_t cpu_zp_i = 0;
        cpu_zp_i < (int32_t)zpolygons_to_render->size;
        cpu_zp_i++)
    {
        if (
            zpolygons_to_render->cpu_data[cpu_zp_i].deleted ||
            !zpolygons_to_render->cpu_data[cpu_zp_i].visible ||
            !zpolygons_to_render->cpu_data[cpu_zp_i].committed ||
            !zpolygons_to_render->cpu_data[cpu_zp_i].alpha_blending_enabled)
        {
            continue;
        }
        
        int32_t mesh_id = zpolygons_to_render->cpu_data[cpu_zp_i].mesh_id;
        log_assert(mesh_id >= 0);
        log_assert(mesh_id < (int32_t)all_mesh_summaries_size);
        
        int32_t vert_tail_i =
            all_mesh_summaries[mesh_id].vertices_head_i +
                all_mesh_summaries[mesh_id].vertices_size;
        assert(vert_tail_i < MAX_VERTICES_PER_BUFFER);
        
        for (
            int32_t vert_i = all_mesh_summaries[mesh_id].vertices_head_i;
            vert_i < vert_tail_i;
            vert_i += 1)
        {
            frame_data->vertices[frame_data->vertices_size].locked_vertex_i =
                vert_i;
            frame_data->vertices[frame_data->vertices_size].polygon_i =
                cpu_zp_i;
            frame_data->vertices_size += 1;
            log_assert(frame_data->vertices_size < ALL_LOCKED_VERTICES_SIZE);
        }
    }
}

inline static void add_opaque_zpolygons_to_workload(
    GPUDataForSingleFrame * frame_data)
{
    assert(frame_data->polygon_collection->size == 0);
    assert(frame_data->vertices_size == 0);
    
    memcpy(
        /* void * dest: */
            frame_data->polygon_collection,
        /* const void * src: */
            zpolygons_to_render->gpu_data,
        /* size_t n: */
            sizeof(GPUPolygon) * zpolygons_to_render->size);
    frame_data->polygon_collection->size = zpolygons_to_render->size;
    
    log_assert(
        frame_data->polygon_collection->size <= zpolygons_to_render->size);
    log_assert(
        zpolygons_to_render->size < MAX_POLYGONS_PER_BUFFER);
    log_assert(
        frame_data->polygon_collection->size < MAX_POLYGONS_PER_BUFFER);
    
    memcpy(
        /* void *__dst: */
            frame_data->polygon_materials,
        /* const void *__src: */
            zpolygons_to_render->gpu_materials,
        /* size_t __n: */
            sizeof(GPUPolygonMaterial) *
                MAX_MATERIALS_PER_POLYGON *
                zpolygons_to_render->size);
    
    for (
        int32_t cpu_zp_i = 0;
        cpu_zp_i < (int32_t)zpolygons_to_render->size;
        cpu_zp_i++)
    {
        if (
            zpolygons_to_render->cpu_data[cpu_zp_i].deleted ||
            !zpolygons_to_render->cpu_data[cpu_zp_i].visible ||
            !zpolygons_to_render->cpu_data[cpu_zp_i].committed ||
            zpolygons_to_render->cpu_data[cpu_zp_i].alpha_blending_enabled)
        {
            continue;
        }
        
        int32_t mesh_id = zpolygons_to_render->cpu_data[cpu_zp_i].mesh_id;
        log_assert(mesh_id >= 0);
        log_assert(mesh_id < (int32_t)all_mesh_summaries_size);
        
        int32_t vert_tail_i =
            all_mesh_summaries[mesh_id].vertices_head_i +
                all_mesh_summaries[mesh_id].vertices_size;
        assert(vert_tail_i < MAX_VERTICES_PER_BUFFER);
        
        for (
            int32_t vert_i = all_mesh_summaries[mesh_id].vertices_head_i;
            vert_i < vert_tail_i;
            vert_i += 1)
        {
            frame_data->vertices[frame_data->vertices_size].locked_vertex_i =
                vert_i;
            frame_data->vertices[frame_data->vertices_size].polygon_i =
                cpu_zp_i;
            frame_data->vertices_size += 1;
            log_assert(frame_data->vertices_size < ALL_LOCKED_VERTICES_SIZE);
        }
    }
}

void hardware_render(
    GPUDataForSingleFrame * frame_data,
    uint64_t elapsed_nanoseconds)
{
    (void)elapsed_nanoseconds;
    
    if (renderer_initialized != true) {
        log_append("renderer not initialized, aborting...\n");
        return;
    }
    
    if (
        frame_data == NULL ||
        frame_data->vertices == NULL)
    {
        log_append("ERROR: platform layer didnt pass recipients\n");
        return;
    }
    
    log_assert(zpolygons_to_render->size < MAX_POLYGONS_PER_BUFFER);
    
    add_opaque_zpolygons_to_workload(frame_data);
    
    if (application_running) {
        add_particle_effects_to_workload(
            /* GPUDataForSingleFrame * frame_data: */
                frame_data,
            /* uint64_t elapsed_nanoseconds: */
                elapsed_nanoseconds,
            /* const uint32_t alpha_blending: */
                false);
        
        add_lineparticle_effects_to_workload(
            frame_data,
            elapsed_nanoseconds,
            false);
    }
    
    frame_data->first_alphablend_i = frame_data->vertices_size;
    
    add_alphablending_zpolygons_to_workload(frame_data);
    
    add_particle_effects_to_workload(
        /* GPUDataForSingleFrame * frame_data: */
            frame_data,
        /* uint64_t elapsed_nanoseconds: */
            elapsed_nanoseconds,
        /* const uint32_t alpha_blending: */
            true);
    
    add_lineparticle_effects_to_workload(
            frame_data,
            elapsed_nanoseconds,
            true);
    
    if (application_running) {
        zpolygon_hitboxes_to_lines(
            frame_data);
    }
    
    if (window_globals->wireframe_mode) {
        frame_data->first_alphablend_i = 0;
    }
    
    if (window_globals->debug_lights_mode) {
        for (
            uint32_t i = 0;
            i < frame_data->light_collection->lights_size;
            i++)
        {
            // TODO: draw light hitboxes
        }
    }
}
