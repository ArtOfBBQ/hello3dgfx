#include "clientlogic.h"

TextureArray texture_arrays[TEXTUREARRAYS_SIZE];
zPolygon * zpolygons_to_render[ZPOLYGONS_TO_RENDER_ARRAYSIZE];
uint32_t zpolygons_to_render_size = 0;

TexQuad texquads_to_render[TEXQUADS_TO_RENDER_ARRAYSIZE];
uint32_t texquads_to_render_size = 0;

zLightSource zlights_to_apply[ZLIGHTS_TO_APPLY_ARRAYSIZE];
uint32_t zlights_to_apply_size = 0;

zPolygon * load_from_obj_file(char * filename)
{
    FileBuffer * buffer = platform_read_file(filename);
    
    zPolygon * return_value = parse_obj(
        /* rawdata     : */ buffer->contents,
        /* rawdata_size: */ buffer->size);
    
    free(buffer->contents);
    free(buffer);
    
    return return_value;
}

void client_logic_startup() {
    printf("client_logic_startup()\n");    
    // These are some example texture atlases we're using for
    // texture mapping on cards and cubes
    assert(TEXTUREARRAYS_SIZE > 0);
    
    texture_arrays[0].sprite_columns = 16;
    texture_arrays[0].sprite_rows = 16;
    texture_arrays[0].request_update = false;
    
    texture_arrays[1].sprite_columns = 3;
    texture_arrays[1].sprite_rows = 2;
    texture_arrays[1].request_update = false;
    
    texture_arrays[2].sprite_columns = 1;
    texture_arrays[2].sprite_rows = 1;
    texture_arrays[2].request_update = false;
    
    FileBuffer * file_buffer;
    
    #define TEXTURE_FILENAMES_SIZE 3
    assert(TEXTURE_FILENAMES_SIZE <= TEXTUREARRAYS_SIZE);
    char * texture_filenames[TEXTURE_FILENAMES_SIZE] = {
        "phoebus.png",
        "sampletexture.png",
        "town.png"};
    for (
        uint32_t i = 0;
        i < TEXTURE_FILENAMES_SIZE;
        i++)
    {
        assert(i < TEXTUREARRAYS_SIZE);
        printf(
            "trying to read file: %s\n",
            texture_filenames[i]);
        file_buffer = platform_read_file(
            texture_filenames[i]);
        if (file_buffer == NULL) {
            printf(
                "ERROR: failed to read file from disk: %s\n",
                texture_filenames[i]);
            assert(false);
        }
        texture_arrays[i].image = decode_PNG(
            (uint8_t *)file_buffer->contents,
            file_buffer->size);
        free(file_buffer->contents);
        free(file_buffer);
        printf(
            "read texture %s with width %u\n",
            texture_filenames[i],
            texture_arrays[i].image->width);
    }
    
    // initialize global zLightSource objects, to set up
    // our lighting for the scene
    zlights_to_apply[0].x = 50.0f;
    zlights_to_apply[0].y = -10.0f;
    zlights_to_apply[0].z = 200.0f;
    zlights_to_apply[0].RGBA[0] = 1.0f;
    zlights_to_apply[0].RGBA[1] = 1.0f;
    zlights_to_apply[0].RGBA[2] = 1.0f;
    zlights_to_apply[0].RGBA[3] = 1.0f;
    zlights_to_apply[0].reach = 15.0f;
    zlights_to_apply[0].ambient = 0.05;
    zlights_to_apply[0].diffuse = 4.0;
    zlights_to_apply_size += 1;
    
    texquads_to_render_size += 1;
    texquads_to_render[0].texturearray_i = -1;
    texquads_to_render[0].texture_i = -1;
    texquads_to_render[0].RGBA[0] = 1.0f;
    texquads_to_render[0].RGBA[1] = 0.0f;
    texquads_to_render[0].RGBA[2] = 0.0f;
    texquads_to_render[0].RGBA[3] = 1.0f;
    texquads_to_render[0].left = -0.5f;
    texquads_to_render[0].width = 1.0f;
    texquads_to_render[0].top = 0.5f;
    texquads_to_render[0].height = 1.0f;
    texquads_to_render[0].visible = true;
    
    printf("finished client_logic_startup()\n");    
}

void client_handle_keypresses(
    uint64_t microseconds_elapsed)
{
    float elapsed_mod =
        (float)((double)microseconds_elapsed / (double)16666);
    float cam_speed = 0.25f * elapsed_mod;
    float cam_rotation_speed = 0.05f * elapsed_mod;
    
    if (keypress_map[0] == true)
    {
        // 'A' key is pressed
        camera.x_angle += cam_rotation_speed;
    }
    
    if (keypress_map[6] == true)
    {
        // 'Z' key is pressed
        camera.z_angle -= cam_rotation_speed;
    }
    
    if (keypress_map[7] == true)
    {
       // 'X' key
       camera.z_angle += cam_rotation_speed;
    }
    
    if (keypress_map[12] == true)
    {
        // 'Q' key is pressed
        camera.x_angle -= cam_rotation_speed;
    }
    
    if (keypress_map[123] == true)
    {
        // left arrow key
        camera.y_angle -= cam_rotation_speed;
        // camera.x -= cam_speed;
    }
    
    if (keypress_map[124] == true)
    {
        // right arrow key
        camera.y_angle += cam_rotation_speed;
        // camera.x += cam_speed;
    }
    
    if (keypress_map[125] == true)
    {
        // down arrow key
        camera.z -= cosf(camera.y_angle) * cam_speed;
        camera.x -= sinf(camera.y_angle) * cam_speed;
    }
    
    if (keypress_map[126] == true)
    {
        // up arrow key is pressed
        zcamera_move_forward(
            &camera,
            cam_speed);
    }
    
    if (keypress_map[30] == true)
    {
        // ] is pressed
    }
    
    if (keypress_map[42] == true)
    {
        // [ is pressed
    }
    
    if (keypress_map[46] == true)
    {
        // m key is pressed
    }
}

void client_handle_touches(
    uint64_t microseconds_elapsed)
{
    float elapsed_mod =
        (float)((double)microseconds_elapsed / (double)16666);
    float cam_speed = 0.25f * elapsed_mod;
    float cam_rotation_speed = 0.05f * elapsed_mod;
    
    // handle tablet & phone touches
    if (!current_touch.handled) {
        if (current_touch.finished) {
            // an unhandled, finished touch
            if (
                (current_touch.finished_at
                    - current_touch.started_at) < 7500)
            {
                if (current_touch.current_y >
                    (window_height * 0.5))
                {
                    camera.z -= 3.0f;
                } else {
                    camera.z += 3.0f;
                }
                current_touch.handled = true;
            } else {
                float delta_x = current_touch.current_x -
                    current_touch.start_x;
                
                if (delta_x < -50.0 || delta_x > 50.0) {
                    camera.y_angle -= (delta_x * 0.001f);
                }
                current_touch.handled = true;
            }
        }
    }
}

bool32_t fading_out = true;
void client_logic_update(
    uint64_t microseconds_elapsed)
{ 
    // TODO: microseconds_elapsed is overridden here because
    // TODO: our timer is weirdly broken on iOS. Fix it!
    microseconds_elapsed = 16666;
    uint64_t fps = 1000000 / microseconds_elapsed;
    float elapsed_mod =
        (float)((double)microseconds_elapsed / (double)16666);
    
    client_handle_keypresses(microseconds_elapsed);
    client_handle_touches(microseconds_elapsed);

    if (texquads_to_render[0].RGBA[3] > 0.95f) {
        printf("fading out..\n");
        fading_out = true;
    }
    
    if (texquads_to_render[0].RGBA[3] < 0.05f) {
        printf("fading in..\n");
        fading_out = false;
    }
    
    if (fading_out) {
        texquads_to_render[0].RGBA[3] -= 0.01f;
    } else {
        texquads_to_render[0].RGBA[3] += 0.01f;
    }
}

