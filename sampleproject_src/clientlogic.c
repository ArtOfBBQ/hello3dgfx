#include "clientlogic.h"

#define TEAPOT 0
#if TEAPOT
static int32_t teapot_mesh_id = -1;
static int32_t teapot_object_ids[2];
static int32_t teapot_touchable_ids[2];
#endif

void client_logic_init(void) {
    
}

void client_logic_early_startup(
    bool32_t * success,
    char * error_message)
{
    const char * fontfile = "font.png";
    if (platform_resource_exists("font.png")) {
        register_new_texturearray_by_splitting_file(
            /* filename : */ fontfile,
            /* rows     : */ 10,
            /* columns  : */ 10);
    } else {
        log_assert(0);
    }
    
    char * filenames[5];
    filenames[0] = malloc_from_unmanaged(64);
    common_strcpy_capped(filenames[0], 64, "structuredart1.png");
    register_new_texturearray_from_files((const char **)filenames, 1);
    
    #if TEAPOT
    // teapot_mesh_id = BASIC_CUBE_MESH_ID;
    teapot_mesh_id = new_mesh_id_from_resource("cardstand.obj");
    #endif
}

void client_logic_late_startup(void) {
    
    zLightSource * light = next_zlight();
    light->RGBA[0]       =  0.70f;
    light->RGBA[1]       =  0.25f;
    light->RGBA[2]       =  0.25f;
    light->RGBA[3]       =  1.00f;
    light->ambient       =  0.0f;
    light->diffuse       =  1.50f;
    light->reach         =  3.00f;
    light->xyz[0]        = -1.25f;
    light->xyz[1]        =  1.00f;
    light->xyz[2]        =  0.10f;
    commit_zlight(light);
    
    light = next_zlight();
    light->RGBA[0]       =  0.05f;
    light->RGBA[1]       =  0.55f;
    light->RGBA[2]       =  0.05f;
    light->RGBA[3]       =  1.00f;
    light->ambient       =  0.0f;
    light->diffuse       =  1.50f;
    light->reach         =  3.00f;
    light->xyz[0]        =  1.55f;
    light->xyz[1]        =  0.60f;
    light->xyz[2]        =  0.10f;
    commit_zlight(light);
    
    #if TEAPOT
    teapot_object_ids[0] = next_nonui_object_id();
    teapot_object_ids[1] = next_nonui_object_id();
    
    for (uint32_t i = 0; i < 2; i++) {
        PolygonRequest teapot_request;
        teapot_request.materials_size = 1;
        request_next_zpolygon(&teapot_request);
        construct_zpolygon(&teapot_request);
        teapot_request.cpu_data->mesh_id = teapot_mesh_id;
        //    scale_zpolygon_multipliers_to_height(
        //        teapot_request.cpu_data,
        //        teapot_request.gpu_data,
        //        0.25f);
        teapot_request.gpu_data->xyz_multiplier[0] = 0.35f;
        teapot_request.gpu_data->xyz_multiplier[1] = 0.35f;
        teapot_request.gpu_data->xyz_multiplier[2] = 0.35f;
        teapot_request.gpu_data->xyz[0]            = 0.00f + (i * 0.20f);
        teapot_request.gpu_data->xyz[1]            = 0.00f + (i * 0.10f);
        teapot_request.gpu_data->xyz[2]            = 5.00f - (i * 0.25f);
        teapot_request.gpu_data->xyz_angle[0]      = 0.00f;
        teapot_request.gpu_data->xyz_angle[1]      = 0.00f;
        teapot_request.gpu_data->xyz_angle[2]      = 0.00f;
        teapot_request.cpu_data->object_id         = teapot_object_ids[i];
        teapot_request.cpu_data->visible           = true;
        teapot_touchable_ids[i]                    = next_nonui_touchable_id();
        teapot_request.cpu_data->touchable_id      = teapot_touchable_ids[i];
        for (uint32_t mat_i = 0; mat_i < MAX_MATERIALS_PER_POLYGON; mat_i++) {
            teapot_request.gpu_materials[mat_i].rgba[0]        =  0.4f;
            teapot_request.gpu_materials[mat_i].rgba[1]        =  0.4f;
            teapot_request.gpu_materials[mat_i].rgba[2]        =  0.4f;
            teapot_request.gpu_materials[mat_i].rgba[3]        =  1.0f;
            teapot_request.gpu_materials[mat_i].texturearray_i = -1.0f;
            teapot_request.gpu_materials[mat_i].texture_i      = -1.0f;
            teapot_request.gpu_materials[mat_i].specular       =  1.0f;
            teapot_request.gpu_materials[mat_i].diffuse        =  1.0f;
        }
        teapot_request.gpu_data->ignore_lighting =  0.0f;
        teapot_request.gpu_data->ignore_camera =  0.0f;
        log_assert(teapot_request.gpu_data->xyz_offset[0] == 0.0f);
        log_assert(teapot_request.gpu_data->xyz_offset[1] == 0.0f);
        log_assert(teapot_request.gpu_data->xyz_offset[2] == 0.0f);
        log_assert(teapot_request.gpu_data->xyz_angle[0] == 0.0f);
        log_assert(teapot_request.gpu_data->xyz_angle[1] == 0.0f);
        log_assert(teapot_request.gpu_data->xyz_angle[2] == 0.0f);
        commit_zpolygon_to_render(&teapot_request);
    }
    #endif
    
    #if 0
    PolygonRequest quad;
    request_next_zpolygon(&quad);
    construct_quad(
        /* const float left_x: */ -0.5f,
        /* const float bottom_y: */ 0.25f,
        /* const float z: */ 1.5f,
        /* const float width: */ 1.8f,
        /* const float height: */ 0.4f,
        /* PolygonRequest * stack_recipient: */ &quad);
    quad.gpu_materials->texturearray_i = 1;
    quad.gpu_materials->texture_i      = 0;
    quad.cpu_data->object_id           = 20;
    quad.cpu_data->touchable_id        = 5;
    quad.gpu_data->xyz_offset[0]       = 0.0f;
    quad.gpu_data->xyz_offset[1]       = 0.0f;
    quad.gpu_data->xyz_offset[2]       = 0.0f;
    quad.gpu_data->scale_factor        = 1.0f;
    quad.gpu_data->xyz_angle[0]        = 0.0f;
    quad.gpu_data->xyz_angle[1]        = 0.0f;
    quad.gpu_data->xyz_angle[2]        = 0.0f;
    quad.gpu_data->ignore_camera       = 1.0f;
    quad.gpu_materials[0].rgba[3]      = 1.0f;
    commit_zpolygon_to_render(&quad);
    #endif
    
    font_settings->font_height = 60;
    font_settings->font_touchable_id = 6;
    font_settings->remove_hitbox = false;
    font_settings->ignore_camera = false;
    text_request_label_renderable(
        /* const int32_t with_object_id: */
            21,
        /* const char * text_to_draw: */
            "Hello!",
        /* const float left_pixelspace: */
             50.0f,
        /* const float top_pixelspace: */
            900.0f,
        /* const float z: */
            3.0f,
        /* const float max_width: */
            500.0f);
    font_settings->font_touchable_id = -1;
    log_assert(
        zpolygons_to_render->cpu_data[zpolygons_to_render->size-1].
            object_id == 21);
}

void client_logic_threadmain(int32_t threadmain_id) {
    switch (threadmain_id) {
        default:
            log_append("unhandled threadmain_id: ");
            log_append_int(threadmain_id);
            log_append("\n");
    }
}

void client_logic_animation_callback(
    const int32_t callback_id,
    const float arg_1,
    const float arg_2,
    const int32_t arg_3)
{
    #ifndef LOGGER_IGNORE_ASSERTS
    char unhandled_callback_id[256];
    common_strcpy_capped(
        unhandled_callback_id,
        256,
        "unhandled client_logic_animation_callback: ");
    common_strcat_int_capped(
        unhandled_callback_id,
        256,
        callback_id);
    common_strcat_capped(
        unhandled_callback_id,
        256,
        ". Find in clientlogic.c -> client_logic_animation_callback\n");
    log_append(unhandled_callback_id);
    log_dump_and_crash(unhandled_callback_id);
    #endif
}

static void client_handle_keypresses(
    uint64_t microseconds_elapsed)
{
    float elapsed_mod = (float)(
        (double)microseconds_elapsed / (double)16666);
    float cam_speed = 0.1f * elapsed_mod;
    float cam_rotation_speed = 0.05f * elapsed_mod;
    
    if (
        keypress_map[TOK_KEY_ENTER] &&
        keypress_map[TOK_KEY_CONTROL])
    {
        keypress_map[TOK_KEY_ENTER] = false;
        platform_toggle_fullscreen();
    }
    
    if (keypress_map[TOK_KEY_S] == true)
    {
        keypress_map[TOK_KEY_S] = false;
        
        #if TEAPOT
        request_shatter_and_destroy(
            /* const int32_t object_id: */
                teapot_object_ids[1],
            /* const uint64_t duration_microseconds: */
                750000);
        #endif
    }
    
    if (keypress_map[TOK_KEY_LEFTARROW] == true)
    {
        camera.xyz[0] -= cam_speed;
    }
    
    if (keypress_map[TOK_KEY_RIGHTARROW] == true)
    {
        camera.xyz[0] += cam_speed;
    }
    
    if (keypress_map[TOK_KEY_DOWNARROW] == true)
    {
        camera.xyz[1] -= cam_speed;
    }
    
    if (keypress_map[TOK_KEY_UPARROW] == true)
    {
        camera.xyz[1] += cam_speed;
    }
    
    if (keypress_map[TOK_KEY_A] == true) {
        camera.xyz_angle[0] += cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_Z] == true) {
        camera.xyz_angle[2] -= cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_X] == true) {
        camera.xyz_angle[2] += cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_Q] == true) {
        camera.xyz_angle[0] -= cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_W] == true) {
        camera.xyz_angle[1] -= cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_S] == true) {
        camera.xyz_angle[1] += cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_BACKSLASH] == true) {
        // / key
        camera.xyz[2] -= 0.01f;
    }
    
    if (keypress_map[TOK_KEY_FULLSTOP] == true) {
        camera.xyz[2] += 0.01f;
    }
}

void client_logic_update(uint64_t microseconds_elapsed)
{
    if (
        !user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].handled)
    {
        user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
            handled = true;
        
        if (
            user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
                touchable_id_top == 5 ||
            user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
                touchable_id_pierce == 5)
        {
            request_bump_animation(
                /* const int32_t object_id: */
                    20,
                /* const uint32_t wait: */
                    0);
        }
        
        if (
            user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
                touchable_id_top == 6 ||
            user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
                touchable_id_pierce == 6)
        {
            request_bump_animation(
                /* const int32_t object_id: */
                    21,
                /* const uint32_t wait: */
                    0);
        }
    }
    
    if (keypress_map[TOK_KEY_R]) {
        for (uint32_t i = 0; i < zpolygons_to_render->size; i++) {
            if (zpolygons_to_render->cpu_data[i].object_id == 20)
            {
                zpolygons_to_render->gpu_data[i].xyz_angle[0] += 0.014f;
                zpolygons_to_render->gpu_data[i].xyz_angle[1] += 0.01f;
                zpolygons_to_render->gpu_data[i].xyz_angle[2] += 0.003f;
            }
        }
    }
    
    if (
        !user_interactions[INTR_PREVIOUS_RIGHTCLICK_START].handled)
    {
        user_interactions[INTR_PREVIOUS_RIGHTCLICK_START].handled = true;
    }
    
    if (
        !user_interactions[INTR_PREVIOUS_MOUSE_OR_TOUCH_MOVE].handled)
    {
        user_interactions[INTR_PREVIOUS_MOUSE_OR_TOUCH_MOVE].handled = true;
    }
    
    #if TEAPOT
    for (uint32_t i = 0; i < 2; i++) {
    if (
        !user_interactions[INTR_PREVIOUS_LEFTCLICK_START].handled &&
        user_interactions[INTR_PREVIOUS_LEFTCLICK_START].touchable_id_top ==
            teapot_touchable_ids[i])
    {
        user_interactions[INTR_PREVIOUS_LEFTCLICK_START].handled = true;
        
        ScheduledAnimation * anim = next_scheduled_animation(true);
        anim->affected_object_id = teapot_object_ids[i];
        anim->gpu_polygon_vals.scale_factor = 1.2f;
        anim->duration_microseconds = 100000;
        anim->runs = 1;
        commit_scheduled_animation(anim);
        
        anim = next_scheduled_animation(true);
        anim->affected_object_id = teapot_object_ids[i];
        anim->gpu_polygon_vals.scale_factor = 1.0f;
        anim->duration_microseconds = 200000;
        anim->wait_before_each_run = 100000;
        anim->runs = 1;
        commit_scheduled_animation(anim);
    }
    }
    
    if (keypress_map[TOK_KEY_R]) {
        for (uint32_t i = 0; i < zpolygons_to_render->size; i++) {
            if (zpolygons_to_render->cpu_data[i].object_id ==
                teapot_object_ids[0])
            {
                zpolygons_to_render->gpu_data[i].xyz_angle[1] += 0.01f;
            }
        }
    }
    #endif
    
    client_handle_keypresses(microseconds_elapsed);
}

void client_logic_evaluate_terminal_command(
    char * command,
    char * response,
    const uint32_t response_cap)
{
    if (common_are_equal_strings(command, "EXAMPLE COMMAND")) {
        common_strcpy_capped(response, response_cap, "Hello from clientlogic!");
        return;
    }
    
    common_strcpy_capped(
        response,
        response_cap,
        "Unrecognized command - see client_logic_evaluate_terminal_command() "
        "in clientlogic.c");
}

void client_logic_window_resize(
    const uint32_t new_height,
    const uint32_t new_width)
{
    // You're notified that the window is resized!
}

void client_logic_shutdown(void) {
    // Your application shutdown code goes here!
}
