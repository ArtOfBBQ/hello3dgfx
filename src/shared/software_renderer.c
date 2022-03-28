#include "software_renderer.h"

TextureArray texture_arrays[TEXTUREARRAYS_SIZE + 1];

zPolygon * zpolygons_to_render[1000];
uint32_t zpolygons_to_render_size = 0;

void init_renderer() {
    // initialize global texture_arrays for texture mapping 
    assert(TEXTUREARRAYS_SIZE > 0);
    texture_arrays[0].filename = "phoebus.png";
    texture_arrays[0].sprite_columns = 16;
    texture_arrays[0].sprite_rows = 16;
    texture_arrays[1].filename = "sampletexture.png";
    texture_arrays[1].sprite_columns = 3;
    texture_arrays[1].sprite_rows = 2;
    texture_arrays[BITMAP_TEXTUREARRAY_I].filename = "";
    texture_arrays[BITMAP_TEXTUREARRAY_I].sprite_columns = 1;
    texture_arrays[BITMAP_TEXTUREARRAY_I].sprite_rows = 1;
    texture_arrays[MINIMAP2_TEXTUREARRAY_I].filename = "";
    texture_arrays[MINIMAP2_TEXTUREARRAY_I].sprite_columns = 1;
    texture_arrays[MINIMAP2_TEXTUREARRAY_I].sprite_rows = 1;
    
    FileBuffer * file_buffer;
    for (
        uint32_t i = 0;
        i < TEXTUREARRAYS_SIZE;
        i++)
    {
        // these 2 are updated manually, no image file to load
        if (i == BITMAP_TEXTUREARRAY_I) { continue; }
        if (i == MINIMAP2_TEXTUREARRAY_I) { continue; }
        
        printf(
            "trying to read file: %s\n",
            texture_arrays[i].filename);
        file_buffer = platform_read_file(
            texture_arrays[i].filename);
        if (file_buffer == NULL) {
            printf(
                "ERROR: failed to read file from disk: %s\n",
                texture_arrays[i].filename);
            assert(false);
        }
        texture_arrays[i].image = decode_PNG(
            (uint8_t *)file_buffer->contents,
            file_buffer->size);
        free(file_buffer->contents);
        free(file_buffer);
        printf(
            "read texture %s with width %u\n",
            texture_arrays[i].filename,
            texture_arrays[i].image->width);
    }
    
    texture_arrays[BITMAP_TEXTUREARRAY_I].image = &minimap;
    texture_arrays[MINIMAP2_TEXTUREARRAY_I].image = &minimap2;
    
    // initialize zPolygon objects, the 3-D objects we're
    // planning to render
    zpolygons_to_render_size = 0;
    
    // objects part 1: load some cards from object file 
    for (uint32_t i = 0; i < 3; i++) {
        uint32_t last_i = zpolygons_to_render_size;
        zpolygons_to_render_size += 1;
        zpolygons_to_render[last_i] =
            i == 2 ?
            load_from_obj_file("teapot.obj")
            : load_from_obj_file("cardwithuvcoords.obj");
        
        float base_y = i % 2 == 0 ? 0.0f : -5.0f;
        zpolygons_to_render[last_i]->x = -2.0f + (4.0f * i);
        zpolygons_to_render[last_i]->y =
            i == 2 ?
            0.0f
            : base_y + (last_i * 7.0f);
        zpolygons_to_render[last_i]->z = i == 2 ? -25.0f : 28.0f;
        
        // change face texture but not the back texture
        for (
            uint32_t t = 0;
            t < zpolygons_to_render[last_i]->triangles_size;
            t++)
        {
            if (
                zpolygons_to_render[last_i]
                    ->triangles[t].texture_i == 2) 
            {
                zpolygons_to_render[last_i]->triangles[t]
                    .texture_i = i;
            }
        }
        
        // scale_zpolygon(
        //     /* to_scale   : */
        //         zpolygons_to_render[last_i],
        //     /* new_height : */
        //         2.0f);
    }
    
    // objects 2: load some hard-coded cubes
    for (uint32_t i = 3; i < 4; i++) {
        uint32_t last_i = zpolygons_to_render_size;
        zpolygons_to_render[last_i] = get_box();
        scale_zpolygon(
            /* to_scale   : */
                zpolygons_to_render[last_i],
            /* new_height : */
                30.0f);
        zpolygons_to_render_size += 1;
    }
    
    // initialize global zLightSource objects, to set up
    // our lighting for the scene
    zlights_to_apply[0].x = 50.0f;
    zlights_to_apply[0].y = 2.5f;
    zlights_to_apply[0].z = 200.0f;
    zlights_to_apply[0].reach = 1.0f;
    zlights_to_apply[0].ambient = 8.0;
    zlights_to_apply[0].diffuse = 8.0;
    zlights_to_apply_size = 1;
    
    // add a white cube to represent the light source
    zpolygons_to_render_size += 1;
    uint32_t light_i = zpolygons_to_render_size - 1;
    zpolygons_to_render[light_i] = get_box();
    zpolygons_to_render[light_i]->x = zlights_to_apply[0].x;
    zpolygons_to_render[light_i]->y = zlights_to_apply[0].y;
    zpolygons_to_render[light_i]->z = zlights_to_apply[0].z;
    for (
        uint32_t j = 0;
        j < zpolygons_to_render[light_i]->triangles_size;
        j++)
    {
        for (uint32_t v = 0; v < 3; v++) {
            // bright white
            zpolygons_to_render[light_i]->triangles[j].color[0] =
                500.0f;
            zpolygons_to_render[light_i]->triangles[j].color[1] =
                500.0f;
            zpolygons_to_render[light_i]->triangles[j].color[2] =
                500.0f;
            zpolygons_to_render[light_i]->triangles[j].color[3] =
                1.0f;
            zpolygons_to_render[light_i]
                ->triangles[j].texturearray_i = -1;
            zpolygons_to_render[light_i]
                ->triangles[j].texture_i = -1;
        }
    }
    // scale_zpolygon(
    //     /* to_scale  : */ zpolygons_to_render[light_i],
    //     /* new_height: */ 0.5f);
    
    renderer_initialized = true;
}

void free_renderer() {
    for (
        uint32_t i = 0;
        i < zpolygons_to_render_size;
        i++)
    {
        free_zpolygon(zpolygons_to_render[i]);
    }
}

void software_render(
    Vertex * next_gpu_workload,
    uint32_t * next_workload_size)
{
    if (renderer_initialized != true) {
        printf("renderer not initialized, aborting...\n");
        return;
    }

    uint64_t elapsed_since_previous_frame =
        platform_end_timer_get_nanosecs();
    platform_start_timer();
    uint64_t fps = 1000000000 / elapsed_since_previous_frame;
    // printf("fps: %llu\n", fps);
    
    if (
        next_gpu_workload == NULL
        || next_workload_size == NULL)
    {
        printf("ERROR: platform layer didnt pass recipients\n");
        return;
    }
    
    if (zpolygons_to_render_size == 0) {
        printf("there's nothing to render - returning...\n");
        return;
    }
    
    // animate our objects
    for (
        uint32_t i = 0;
        i < (zpolygons_to_render_size);
        i++)
    {
        zpolygons_to_render[i]->x -= 0.005f;
        zpolygons_to_render[i]->z_angle += 0.03f;
        zpolygons_to_render[i]->x_angle += 0.021f;
        zpolygons_to_render[i]->y_angle += 0.015f;
    }
    
    // move our light source
    uint32_t light_i = zpolygons_to_render_size - 1;
    zpolygons_to_render[light_i]->y -= 0.001;
    if (
        zpolygons_to_render[light_i]->z > 20.0f)
    {
        zpolygons_to_render[light_i]->z -= 1.2;
        zpolygons_to_render[light_i]->x -= 0.14;
    }
    zlights_to_apply[0].x = zpolygons_to_render[light_i]->x;
    zlights_to_apply[0].y = zpolygons_to_render[light_i]->y;
    zlights_to_apply[0].z = zpolygons_to_render[light_i]->z;
    
    uint32_t triangles_to_render = 0;
    for (
        uint32_t i = 0;
        i < zpolygons_to_render_size;
        i++)
    {
        for (
            uint32_t j = 0;
            j < zpolygons_to_render[i]->triangles_size;
            j++)
        {
            triangles_to_render++;
        }
    }
    
    if (triangles_to_render == 0) { return; }
    
    // transform all triangles
    zTriangle triangles_to_draw[triangles_to_render];
    zTriangle position_translated;
    zTriangle camera_y_rotated;
    zTriangle camera_x_rotated;
    zTriangle camera_z_rotated;
    zTriangle camera_translated;
    zTriangle x_rotated;
    zTriangle y_rotated;
    zTriangle z_rotated;
    minimaps_clear();
    uint32_t t = 0;
    for (
        uint32_t i = 0;
        i < zpolygons_to_render_size;
        i++)
    {
        decodedimg_add_zpolygon(
            &minimap2,
            zpolygons_to_render[i]);
        
        for (
            uint32_t j = 0;
            j < zpolygons_to_render[i]->triangles_size;
            j++)
        {
            assert(t < triangles_to_render);
            
            x_rotated = x_rotate_triangle(
                zpolygons_to_render[i]->triangles + j,
                zpolygons_to_render[i]->x_angle);
            y_rotated = y_rotate_triangle(
                &x_rotated,
                zpolygons_to_render[i]->y_angle);
            z_rotated = z_rotate_triangle(
                &y_rotated,
                zpolygons_to_render[i]->z_angle);
            
            position_translated = translate_ztriangle(
                /* input: */
                    &z_rotated,
                /* x: */
                    zpolygons_to_render[i]->x - camera.x,
                /* y: */
                    zpolygons_to_render[i]->y - camera.y,
                /* z: */
                    zpolygons_to_render[i]->z - camera.z);
            
            camera_y_rotated = y_rotate_triangle(
                &position_translated,
                -camera.y_angle);
            camera_x_rotated = x_rotate_triangle(
                &camera_y_rotated,
                -camera.x_angle);
            camera_z_rotated = z_rotate_triangle(
                &camera_x_rotated,
                -camera.z_angle);
            
            decodedimg_add_triangle(&minimap, &camera_z_rotated);
            triangles_to_draw[t] = camera_z_rotated;
            t++;
        }
    }
    
    // minimap_add_camera(&camera);
    zCamera imaginary_camera_at_origin;
    imaginary_camera_at_origin.x = 0.0f;
    imaginary_camera_at_origin.y = 0.0f;
    imaginary_camera_at_origin.z = 0.0f;
    imaginary_camera_at_origin.x_angle = 0.0f;
    imaginary_camera_at_origin.y_angle = 0.0f;
    imaginary_camera_at_origin.z_angle = 0.0f;
    decodedimg_add_camera(&minimap, &imaginary_camera_at_origin);
    decodedimg_add_camera(&minimap2, &camera);
    
    // sort all triangles so the most distant ones can be
    // drawn first 
    qsort(
        triangles_to_draw,
        triangles_to_render,
        sizeof(zTriangle),
        &sorter_cmpr_lowest_z);
    
    for (
        int32_t i = triangles_to_render - 1;
        i >= 0;
        i -= 1)
    {
        // we're not using the camera because the entire world
        // was translated to have the camera be at 0,0,0
        zVertex origin;
        origin.x = 0.0f;
        origin.y = 0.0f;
        origin.z = 0.0f; 
        float perspective_dot_product =
            get_visibility_rating(
                origin,
                triangles_to_draw + i,
                0);
        
        if (perspective_dot_product < 0.0f
            && triangles_to_draw[i].vertices[0].z
                > projection_constants.near
            && triangles_to_draw[i].vertices[1].z
                > projection_constants.near
            && triangles_to_draw[i].vertices[2].z
                > projection_constants.near)
        {
            Vertex triangle_to_draw[3];
           
            // TODO: translate all lights in advance,
            // instead of repeating the work for every triangle
            // drawn
            for (
                uint32_t l = 0;
                l < zlights_to_apply_size;
                l++)
            {
                zVertex translated_light_pos;
                translated_light_pos.x =
                    zlights_to_apply[l].x - camera.x;
                translated_light_pos.y =
                    zlights_to_apply[l].y - camera.y;
                translated_light_pos.z =
                    zlights_to_apply[l].z - camera.z;
                translated_light_pos = x_rotate_zvertex(
                    &translated_light_pos,
                    -1 * camera.x_angle);
                translated_light_pos = y_rotate_zvertex(
                    &translated_light_pos,
                    -1 * camera.y_angle);
                translated_light_pos = z_rotate_zvertex(
                    &translated_light_pos,
                    -1 * camera.z_angle);
                
                zLightSource translated_light =
                    zlights_to_apply[l];
                translated_light.x = translated_light_pos.x;
                translated_light.y = translated_light_pos.y;
                translated_light.z = translated_light_pos.z;
                
                ztriangle_apply_lighting(
                    /* recipient: */
                        triangle_to_draw,
                    /* input: */
                        triangles_to_draw + i,
                    /* zlight_source: */
                        &translated_light);
            }
            
            ztriangle_to_2d(
                /* recipient: */
                    triangle_to_draw,
                /* input: */
                    triangles_to_draw + i);
            
            draw_triangle(
                /* vertices_recipient: */
                    next_gpu_workload,
                /* vertex_count_recipient: */
                    next_workload_size,
                /* input: */
                    triangle_to_draw);
        }
    }
}

void rotate_triangle(
    Vertex to_rotate[3],
    const float angle)
{
    for (uint32_t i = 0; i < 3; i++) {
        to_rotate[i].x = 
            (cosf(angle) * to_rotate[i].x)
                + (sinf(angle) * to_rotate[i].y);
        to_rotate[i].y =
            (-sinf(angle) * to_rotate[i].x)
                + (cosf(angle) * to_rotate[i].y);
    }
}

zPolygon * load_from_obj_file(char * filename)
{
    FileBuffer * buffer = platform_read_file(filename);
    
    zPolygon * return_value = load_from_obj(
        /* rawdata     : */ buffer->contents,
        /* rawdata_size: */ buffer->size);

    free(buffer->contents);
    free(buffer);
    
    return return_value;
}

