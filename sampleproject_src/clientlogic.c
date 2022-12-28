#include "clientlogic.h"

#define TEXTURE_FILENAMES_SIZE 6
DecodedImage * decoded_pngs[TEXTURE_FILENAMES_SIZE];

int32_t latest_object_id = 72;

typedef struct TextureArrayLocation {
    int32_t texturearray_i;
    int32_t texture_i;
} TextureArrayLocation;


static zPolygon card_model;
zPolygon load_from_obj_file_expecting_materials(
    char * filepath,
    ExpectedObjMaterials * expected_materials,
    const uint32_t expected_materials_size,
    const bool32_t flip_winding)
{
    log_append("loading obj file: ");
    log_append(filepath);
    log_append_char('\n');
    
    FileBuffer buffer;
    buffer.size = (uint64_t)platform_get_resource_size(filepath) + 1;
    buffer.contents = (char *)malloc_from_managed(buffer.size);
    platform_read_resource_file(
        filepath,
        &buffer);
    
    assert(buffer.size > 1);
    
    zPolygon return_value = parse_obj_expecting_materials(
        /* rawdata     : */ buffer.contents,
        /* rawdata_size: */ buffer.size,
        expected_materials,
        expected_materials_size,
        flip_winding);
    
    free_from_managed((uint8_t *)buffer.contents);
    
    return return_value;
}

zPolygon load_from_obj_file(char * filepath, const bool32_t flip_winding) {
    FileBuffer buffer;
    buffer.size = (uint64_t)platform_get_resource_size(filepath) + 1;
    buffer.contents = (char *)malloc_from_managed(buffer.size);
    platform_read_resource_file(
        filepath,
        &buffer);
    
    assert(buffer.size > 1);
    
    zPolygon return_value = parse_obj(
        /* rawdata     : */ buffer.contents,
        /* rawdata_size: */ buffer.size,
        flip_winding);
    
    free_from_managed((uint8_t *)buffer.contents);
    
    return return_value;
}

void client_logic_get_application_name_to_recipient(
    char * recipient,
    const uint32_t recipient_size)
{
    char * app_name = (char *)"TOK ONE";
    
    strcpy_capped(
        /* recipient: */ recipient,
        /* recipient_size: */ recipient_size,
        /* origin: */ app_name);
}

static bool32_t ran_anim[4]; // 1 for each card in the test scene
void client_logic_startup() {
    
    // camera.z_angle = 0.3f;
    
    ran_anim[0] = false;
    ran_anim[1] = false;
    ran_anim[2] = false;
    ran_anim[3] = false;
    
    init_rand_with_seed(platform_get_current_time_microsecs());
    
    const char * fontfile;
    fontfile = "font.png";
    register_new_texturearray_by_splitting_file(
        /* filename : */ fontfile,
        /* rows     : */ 10,
        /* columns  : */ 10);
    
    char * filenames[2] = {
        (char *)"structuredart1.png",
        (char *)"structuredart2.png",
    };
    register_new_texturearray_from_files(
        (const char **)filenames,
        2);
    
    char * moar_filenames[1] = {
        (char *)"roundedborder.png",
    };
    register_new_texturearray_from_files(
        (const char **)moar_filenames,
        1);
    
    char * card_filenames[2] = {
        (char *)"fs_justice.bmp",
        (char *)"fs_cardback.bmp"
    };
    register_new_texturearray_from_files(
        (const char **)card_filenames,
        2);
    
    zlights_to_apply[0].deleted    =   false;
    zlights_to_apply[0].object_id  =      -1;
    zlights_to_apply[0].x          =   -1.2f;
    zlights_to_apply[0].y          =    0.0f;
    zlights_to_apply[0].z          =    0.5f;
    zlights_to_apply[0].RGBA[0]    =    1.0f;
    zlights_to_apply[0].RGBA[1]    =    0.0f;
    zlights_to_apply[0].RGBA[2]    =    0.0f;
    zlights_to_apply[0].RGBA[3]    =    1.0f;
    zlights_to_apply[0].reach      =    1.75f;
    zlights_to_apply[0].ambient    =    0.15f;
    zlights_to_apply[0].diffuse    =    2.5f;
    zlights_to_apply_size++;
    
    zlights_to_apply[1].deleted    =   false;
    zlights_to_apply[1].object_id  =      -1;
    zlights_to_apply[1].x          =    0.8f;
    zlights_to_apply[1].y          =    0.0f;
    zlights_to_apply[1].z          =    0.5f;
    zlights_to_apply[1].RGBA[0]    =    0.0f;
    zlights_to_apply[1].RGBA[1]    =    0.0f;
    zlights_to_apply[1].RGBA[2]    =    1.0f;
    zlights_to_apply[1].RGBA[3]    =    1.0f;
    zlights_to_apply[1].reach      =    1.75f;
    zlights_to_apply[1].ambient    =    0.15f;
    zlights_to_apply[1].diffuse    =    2.5f;
    zlights_to_apply_size++;
    log_assert(zlights_to_apply_size == 2);
        
    zPolygon teapot = load_from_obj_file("teapot.obj", false);
    // center_zpolygon_offsets(&teapot);
    teapot = teapot;
    
    teapot.object_id = 123;
    teapot.touchable_id = 123;
    log_assert(teapot.triangles_size > 0);
    scale_zpolygon(
        /* to_scale: */ &teapot,
        /* new_height: */ 0.5f);
    teapot.x = 0.0f;
    teapot.y = 0.0f;
    teapot.z = 0.8f;
    request_zpolygon_to_render(&teapot);
    
    zPolygon anotherteapot = teapot;
    anotherteapot.object_id = 124;
    anotherteapot.touchable_id = 124;
    anotherteapot.x = teapot.x - 0.3f;
    anotherteapot.y = teapot.y - 0.3f;
    anotherteapot.z = teapot.z - 1.5f;
    request_zpolygon_to_render(&anotherteapot);
    
    ScheduledAnimation rotate_teapot;
    construct_scheduled_animation(&rotate_teapot);
    rotate_teapot.affected_object_id = 123;
    rotate_teapot.z_rotation_per_second = 0.2f;
    rotate_teapot.y_rotation_per_second = 0.5f;
    rotate_teapot.x_rotation_per_second = 0.9f;
    rotate_teapot.duration_microseconds = 1000000;
    rotate_teapot.runs = 0;
    request_scheduled_animation(&rotate_teapot);
    
    TexQuad texture;
    construct_texquad(&texture);
    texture.RGBA[0]       = 1.0f;
    texture.RGBA[1]       = 1.0f;
    texture.RGBA[2]       = 1.0f;
    texture.RGBA[3]       = 1.0f;
    texture.object_id     = 125;
    texture.touchable_id  = 125;
    texture.top_y         = 0.9f;
    texture.left_x        = 0.9f;
    texture.width         = 0.09f;
    texture.height        = 0.09f;
    texture.z_angle       = 0.3f;
    texture.ignore_camera = false;
    request_texquad_renderable(&texture);
    
    log_append("finished client_logic_startup()\n");
}

void client_logic_threadmain(int32_t threadmain_id) {
    switch (threadmain_id) {
        case (0):
            // load_assets(1, texture_arrays_size - 1);
            break;
        default:
            log_append("unhandled threadmain_id: ");
            log_append_int(threadmain_id);
            log_append("\n");
    }
}

void client_logic_animation_callback(int32_t callback_id)
{
    char unhandled_callback_id[256];
    strcpy_capped(unhandled_callback_id, 256, "unhandled client_logic_animation_callback: ");
    strcat_int_capped(unhandled_callback_id, 256, callback_id);
    strcat_capped(unhandled_callback_id, 256, ". You should handle it in clientlogic.c -> client_logic_animation_callback\n");
    log_append(unhandled_callback_id);
    log_dump_and_crash(unhandled_callback_id);
}

static void  client_handle_touches_and_leftclicks(
    uint64_t microseconds_elapsed)
{
    if (!previous_touch_or_leftclick_end.handled) {
        if (previous_touch_or_leftclick_end.touchable_id >= 0) {
            ScheduledAnimation bump;
            construct_scheduled_animation(&bump);
            bump.affected_object_id = previous_touch_or_leftclick_end.touchable_id;
            bump.final_scale_known = true;
            bump.final_scale = 1.2f;
            bump.duration_microseconds = 100000;
            request_scheduled_animation(&bump);
            
            ScheduledAnimation unbump;
            construct_scheduled_animation(&unbump);
            unbump.affected_object_id = previous_touch_or_leftclick_end.touchable_id;
            unbump.final_scale_known = true;
            unbump.final_scale = 1.0f;
            unbump.remaining_wait_before_next_run  = 100000;
            unbump.duration_microseconds = 100000;
            request_scheduled_animation(&unbump);
        }
        previous_touch_or_leftclick_end.handled = true;
    }
}

static void client_handle_keypresses(uint64_t microseconds_elapsed) {
    float elapsed_mod =
        (float)((double)microseconds_elapsed / (double)16666);
    float cam_speed = 0.1f * elapsed_mod;
    float cam_rotation_speed = 0.05f * elapsed_mod;
    
    if (keypress_map[123] == true)
    {
        // left arrow key
        camera.x -= cam_speed;
    }
    
    if (keypress_map[124] == true)
    {
        // right arrow key
        camera.x += cam_speed;
    }
    
    if (keypress_map[125] == true)
    {
        // down arrow key
        camera.y -= cam_speed;
    }
    
    if (keypress_map[126] == true)
    {
        // up arrow key is pressed
        camera.y += cam_speed;
    }
     
    if (keypress_map[0] == true) {                                              
        // 'A' key is pressed                                                   
        camera.x_angle += cam_rotation_speed;                                   
    }
    
    if (keypress_map[6] == true) {                                              
        // 'Z' key is pressed                                                   
        camera.z_angle -= cam_rotation_speed;                                   
    }
    
    if (keypress_map[7] == true) {                                              
        // 'X' key                                                               
        camera.z_angle += cam_rotation_speed;                                    
    }
    
    if (keypress_map[12] == true) {                                             
        // 'Q' key is pressed                                                   
        camera.x_angle -= cam_rotation_speed;                                   
    }
    
    if (keypress_map[44] == true) {                                             
        // / key                                                                
        camera.z -= 0.01f;                                                      
    }
     
    if (keypress_map[94] == true) {                                             
        // _ key is pressed                                                     
        camera.z += 0.01f;                                                      
    }
}

bool32_t fading_out = true;

void client_logic_update(uint64_t microseconds_elapsed)
{
    //    request_fps_counter(microseconds_elapsed);
    
    client_handle_touches_and_leftclicks(microseconds_elapsed);
    client_handle_keypresses(microseconds_elapsed);
    
    // TODO: remove this debug code
    float light_0_to_polygon_0_dist_x =
        (zpolygons_to_render[0].x - zlights_to_apply[0].x) *
        (zpolygons_to_render[0].x - zlights_to_apply[0].x);
    float light_0_to_polygon_0_dist_y =
        (zpolygons_to_render[0].y - zlights_to_apply[0].y) *
        (zpolygons_to_render[0].y - zlights_to_apply[0].y);
    float light_0_to_polygon_0_dist_z =
        (zpolygons_to_render[0].z - zlights_to_apply[0].z) *
        (zpolygons_to_render[0].z - zlights_to_apply[0].z);
    float sum_squares =
        light_0_to_polygon_0_dist_x +
        light_0_to_polygon_0_dist_y +
        light_0_to_polygon_0_dist_z;
    float distance = sqrtf(sum_squares);
    
    log_assert(zlights_to_apply[0].x == -1.2f);
    log_assert(zlights_to_apply[0].y ==  0.0f);
    log_assert(zlights_to_apply[0].z ==  0.5f);
    log_assert(zlights_to_apply[1].x ==  0.8f);
    log_assert(zlights_to_apply[1].y ==  0.0f);
    log_assert(zlights_to_apply[1].z ==  0.5f);  
}

void client_logic_window_resize(
    const uint32_t new_height,
    const uint32_t new_width)
{
    char unhandled_rs_msg[256];
    strcpy_capped(unhandled_rs_msg, 256, "Error: unhandled client_logic_window_resize() to height/width: of ");
    strcat_uint_capped(unhandled_rs_msg, 256, new_height);
    strcat_capped(unhandled_rs_msg, 256, ", ");
    strcat_uint_capped(unhandled_rs_msg, 256, new_width);
    strcat_capped(unhandled_rs_msg, 256, ".\nEither prevent app resizing or handle in clientlogic.c\n");
    log_append(unhandled_rs_msg);
    log_dump_and_crash(unhandled_rs_msg);
}

void client_logic_shutdown(void) {
    // Your application shutdown code goes here!
}
