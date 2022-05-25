#include "clientlogic.h"

#define TEXTURE_FILENAMES_SIZE 6
DecodedImage * decoded_pngs[TEXTURE_FILENAMES_SIZE];

char filenames_for_texturearrays
    [TEXTUREARRAYS_SIZE]
    [MAX_FILES_IN_SINGLE_TEXARRAY]
    [MAX_ASSET_FILENAME_SIZE];
uint32_t filenames_for_texturearrays_size = 0;

typedef struct TextureArrayLocation {
    int32_t texturearray_i;
    int32_t texture_i;
} TextureArrayLocation;

zPolygon load_from_obj_file(char * filename)
{
    FileBuffer buffer;
    buffer.size = platform_get_filesize(filename) + 1;
    char buffer_contents[buffer.size];
    buffer.contents = (char *)&buffer_contents;
    platform_read_file(filename, &buffer);
    
    zPolygon return_value =
        parse_obj(
            /* rawdata     : */ buffer.contents,
            /* rawdata_size: */ buffer.size);
    
    return return_value;
}

uint32_t label_object_id = 0;
uint32_t teapot_object_id = 1;

void preregister_assets() {

    const char * fontfile[1];
    fontfile[0] = "font.png";
    register_new_texturearray_from_files(
        fontfile,
        1);
    texture_arrays[0].sprite_columns = 9;
    texture_arrays[0].sprite_rows = 9;
    texture_arrays[0].request_update = false;
    
    font_height = 40.0f; 
    
    DIR *            FD;
    struct dirent *  fe;
    
    typedef struct {
        char filename[MAX_ASSET_FILENAME_SIZE];
        int32_t texturearray_i;
        int32_t texture_i;
        uint32_t width;
        uint32_t height;
    } Asset;
    
    Asset registered_assets[MAX_ASSET_FILES];
    uint32_t registered_assets_size = 0;
    
    // initialize everything to empty
    for (
        uint32_t ta_i = 1;
        ta_i < texture_arrays_size;
        ta_i++)
    {
        uint32_t t_i = 0;
        while (
            t_i < MAX_FILES_IN_SINGLE_TEXARRAY)
        {
            filenames_for_texturearrays[ta_i][t_i][0] = '\0';
            t_i++;
        }
    }
    
    /* Scanning the in directory */
    if (NULL ==
        (FD = opendir(platform_get_application_path())))
    {
        printf(
            "Error : Failed to open app home dir\n");
        assert(0);
    }
    
    while ((fe = readdir(FD)))
    {
        // associate the names with texture arrays first 
        if (fe->d_namlen >= 4 &&
            fe->d_name[fe->d_namlen - 4] == '.' &&
            fe->d_name[fe->d_namlen - 3] == 'p' &&
            fe->d_name[fe->d_namlen - 2] == 'n' &&
            fe->d_name[fe->d_namlen - 1] == 'g')
        {
            if (fe->d_namlen >= MAX_ASSET_FILENAME_SIZE) {
                printf(
                    "asset file %s filename exceeds MAX_ASSET_FILENAME_SIZE [%u] characters\n",
                    fe->d_name,
                    MAX_ASSET_FILENAME_SIZE);
                assert(0);
            }
            
            FileBuffer png_file;
            png_file.size = 40; // read first 40 bytes only
            char png_file_contents[40];
            png_file.contents = (char *)&png_file_contents;
            platform_read_file(
                /* filename: */ fe->d_name,
                /* out_preallocatedbuffer: */ &png_file);
            
            uint32_t width;
            uint32_t height;
            get_PNG_width_height(
                /* compressed_bytes: */
                    (uint8_t *)png_file.contents,
                /* compressed_bytes_size: */
                    50,
                /* width_out: */
                    &width,
                /* height_out: */
                    &height);
            
            assert(width > 0);
            assert(height > 0);
            
            int32_t new_texturearray_i = -1;
            int32_t new_texture_i = -1;
            
            for (
                uint32_t i = 0;
                i < registered_assets_size;
                i++)
            {
                if (registered_assets[i].width == width
                    && registered_assets[i].height == height)
                {
                    assert(
                        new_texturearray_i < 0 ||
                        new_texturearray_i ==
                            registered_assets[i].
                                texturearray_i);
                    new_texturearray_i =
                        registered_assets[i].texturearray_i;
                    if (
                        new_texture_i < registered_assets[i]
                            .texture_i)
                    {
                        new_texture_i =
                            registered_assets[i].texture_i;
                    }
                }
            }
            new_texture_i++;

            if (
                fe->d_name[0] == 'f'
                && fe->d_name[1] == 'o'
                && fe->d_name[2] == 'n'
                && fe->d_name[3] == 't')
            {
                continue;
            }
            
            if (new_texturearray_i < 0) {
                new_texturearray_i =
                    texture_arrays_size;
                filenames_for_texturearrays_size++;
                texture_arrays_size++;
            }
            
            registered_assets[registered_assets_size].width =
                width;
            registered_assets[registered_assets_size].height =
                height;
            registered_assets[registered_assets_size]
                .texturearray_i = new_texturearray_i;
            registered_assets[registered_assets_size]
                .texture_i = new_texture_i;
            
            strcpy(
                registered_assets[registered_assets_size]
                    .filename,
                fe->d_name);
            registered_assets_size += 1;
            
            TextureArrayLocation new_loc;
            new_loc.texturearray_i = new_texturearray_i;
            new_loc.texture_i = new_texture_i;
            
            copy_strings(
                /* recipient: */
                    filenames_for_texturearrays
                        [new_texturearray_i][new_texture_i],
                /* recipient_size: */
                    MAX_ASSET_FILENAME_SIZE,
                /* origin: */
                    fe->d_name,
                /* origin_size: */
                    fe->d_namlen);
        }
    }
}

void load_assets(uint32_t start_i, uint32_t last_i) {
    printf(
        "load_assets from %u to and including %u\n",
        start_i,
        last_i);
        
    for (
        int32_t dimension_i = start_i;
        dimension_i <= last_i;
        dimension_i++)
    {
        printf(
            "loading filenames_for_texturearrays[%u]\n",
            dimension_i);
        
        update_texturearray_from_0terminated_files(
            /* const int32_t texturearray_i: */
                dimension_i,
            /* const char *** filenames: */
                filenames_for_texturearrays[dimension_i]);
    }
}

void client_logic_startup() {
    printf("client_logic_startup()\n");    
    // These are some example texture atlases we're using for
    // texture mapping on cards and cubes
    assert(TEXTUREARRAYS_SIZE > 0);
    
    preregister_assets();
    
    load_assets(1, 3);

    // debug_dump_texturearrays_to_disk();
    // assert(0);
    
    // reminder: threadmain_id 0 calls load_assets() 
    // platform_start_thread(/* threadmain_id: */ 0);

    zlights_to_apply[0].x = 1;
    zlights_to_apply[0].y = 1;
    zlights_to_apply[0].z = 1;
    zlights_to_apply[0].RGBA[0] = 1.0f;
    zlights_to_apply[0].RGBA[1] = 1.0f;
    zlights_to_apply[0].RGBA[2] = 1.0f;
    zlights_to_apply[0].RGBA[3] = 1.0f;
    zlights_to_apply[0].ambient = 5.0f;
    zlights_to_apply[0].reach = 50.0f;
    zlights_to_apply_size++;
    
    TexQuad sample_quad;
    construct_texquad(&sample_quad);
    sample_quad.object_id = 5;
    sample_quad.texturearray_i = 3;
    sample_quad.texture_i = 1;
    sample_quad.left_pixels = 100;
    sample_quad.top_pixels = 400;
    sample_quad.width_pixels = 100;
    sample_quad.height_pixels = 100;
    request_texquad_renderable(&sample_quad);
    
    printf("finished client_logic_startup()\n");    
}

void client_logic_threadmain(int32_t threadmain_id) {
    printf(
        "client_logic_threadmain(%i)\n",
        threadmain_id);
    
    switch (threadmain_id) {
        case (0):
            // load_assets();
            break;
        default:
            printf(
                "unhandled threadmain_id: %i\n",
                threadmain_id);
    }
}

void client_logic_animation_callback(int32_t callback_id)
{
    printf(
        "client_logic_animation_callback(%i)\n",
        callback_id);
}

void client_handle_mouseevents(
    uint64_t microseconds_elapsed)
{
    uint32_t touched_object_id = 999999;
    
    if (!last_mouse_down.handled) {
        last_mouse_down.handled = true;
        if (
            touchable_id_to_texquad_object_id(
                /* const int32_t touchable_id : */
                    last_mouse_down.touchable_id,
                /* uint32_t * object_id_out : */
                    &touched_object_id))
        {
            assert(touched_object_id != 999999);
            
            ScheduledAnimation brighten;
            construct_scheduled_animation(&brighten);
            ScheduledAnimation dim;
            construct_scheduled_animation(&dim);
            
            brighten.affected_object_id = touched_object_id;
            dim.affected_object_id = touched_object_id;
            
            brighten.remaining_microseconds = 150000;
            dim.wait_first_microseconds =
                brighten.remaining_microseconds;
            dim.remaining_microseconds =
                brighten.remaining_microseconds;
            
            for (uint32_t i = 0; i < 3; i++) {
                brighten.rgba_delta_per_second[i] = 0.9;
                dim.rgba_delta_per_second[i] = -0.9;
            }
            brighten.z_rotation_per_second = -0.13;
            dim.z_rotation_per_second = 0.13;
            
            request_scheduled_animation(&brighten);
            request_scheduled_animation(&dim);
        } else {
            printf("touched screen at: [%f,%f]\n",
                last_mouse_down.screenspace_x,
                last_mouse_down.screenspace_y);
            TexQuad touch_highlight;
            construct_texquad(&touch_highlight);
            touch_highlight.object_id = 72;
            touch_highlight.left_pixels = last_mouse_down.screenspace_x;
            touch_highlight.top_pixels = last_mouse_down.screenspace_y;
            touch_highlight.z = 50;
            touch_highlight.height_pixels = 20.0f;
            touch_highlight.width_pixels = 20.0f;
            touch_highlight.RGBA[0] = 1.0f;
            touch_highlight.RGBA[0] = 0.0f;
            touch_highlight.RGBA[0] = 1.0f;
            touch_highlight.RGBA[0] = 1.0f;
            request_texquad_renderable(&touch_highlight);
            
            ScheduledAnimation vanish;
            construct_scheduled_animation(&vanish);
            vanish.affected_object_id = touch_highlight.object_id;
            vanish.remaining_microseconds = 400000;
            vanish.delete_object_when_finished = true;
            vanish.rgba_delta_per_second[3] = -2.5f;
            request_scheduled_animation(&vanish);
        }
    }
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
    //float elapsed_mod =
    //    (float)((double)microseconds_elapsed / (double)16666);
    
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
char fps_string[8] = "fps: xx";

void client_logic_update(
    uint64_t microseconds_elapsed)
{
    // TODO: our timer is weirdly broken on iOS. Fix it!
    uint64_t fps = 1000000 / microseconds_elapsed;
    /*
    float elapsed_mod =
        (float)((double)microseconds_elapsed / (double)16666);
    */
    
    if (fps < 100) {
        fps_string[5] = '0' + ((fps / 10) % 10);
        fps_string[6] = '0' + (fps % 10);
    } else {
        fps_string[5] = '9' + ((fps / 10) % 10);
        fps_string[6] = '9' + (fps % 10);
    }
    
    float fps_color[4];
    fps_color[0] = 0.8f;
    fps_color[1] = 0.2f;
    fps_color[2] = 0.8f;
    fps_color[3] = 1.0f;
    delete_texquad_object(label_object_id);
    
    request_label_renderable(
        /* with_id               : */ label_object_id,
        /* char * text_to_draw   : */ fps_string,
        /* float text_color[4]   : */ fps_color,
        /* text_to_draw_size     : */ 7,
        /* float left_pixelspace : */ 20.0f,
        /* float top_pixelspace  : */ 60.0f,
        /* z                     : */ 0.5f,
        /* float max_width       : */ window_width,
        /* bool32_t ignore_camera: */ true);
    
    client_handle_mouseevents(
        microseconds_elapsed);
    client_handle_keypresses(
        microseconds_elapsed);
    client_handle_touches(
        microseconds_elapsed);
}

