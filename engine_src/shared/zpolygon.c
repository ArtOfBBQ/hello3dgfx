#include "zpolygon.h"

// If you want to draw 3D objects to the screen, you need
// to set them up here
zPolygon * zpolygons_to_render;
uint32_t zpolygons_to_render_size = 0;

static void set_zpolygon_hitbox(
    zPolygon * mesh)
{
    log_assert(mesh->triangles_size > 0);
    
    float top = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;
    float right = 0.0f;
    float back = 0.0f;
    float front = 0.0f;
    
    for (uint32_t i = 0; i < mesh->triangles_size; i++) {
        for (uint32_t m = 0; m < 3; m++) {
            if (mesh->triangles[i].vertices[m].x < left) {
                left = mesh->triangles[i].vertices[m].x;
            } else if (mesh->triangles[i].vertices[m].x > right) {
                right = mesh->triangles[i].vertices[m].x; 
            }
            
            if (mesh->triangles[i].vertices[m].y < bottom) {
                bottom = mesh->triangles[i].vertices[m].y;
            } else if (mesh->triangles[i].vertices[m].y > top) {
                top = mesh->triangles[i].vertices[m].y; 
            }
            
            if (mesh->triangles[i].vertices[m].z < front) {
                front = mesh->triangles[i].vertices[m].z;
            } else if (mesh->triangles[i].vertices[m].z > back) {
                back = mesh->triangles[i].vertices[m].z; 
            }
        }
    }
    
    log_assert(bottom <= 0.0f);
    log_assert(top    >= 0.0f);
    
    log_assert(left   <= 0.0f);
    log_assert(right  >= 0.0f);
    
    log_assert(back   >= 0.0f);
    log_assert(front  <= 0.0f);
    log_assert(back   >= front);
    
    mesh->hitbox_height = (top - bottom) + 0.00001f;
    mesh->hitbox_width  = (right - left) + 0.00001f;
    mesh->hitbox_depth  = (back - front) + 0.00001f;
}

void request_zpolygon_to_render(zPolygon * to_add)
{
    log_assert(to_add->triangles != NULL);
    log_assert(to_add->triangles_size > 0);
    
    for (
        uint32_t tri_i = 0;
        tri_i < to_add->triangles_size;
        tri_i++)
    {
        if (to_add->triangles[tri_i].texturearray_i < 0) {
            log_assert(to_add->triangles[tri_i].texture_i < 0);
        }
        if (to_add->triangles[tri_i].texture_i < 0) {
            log_assert(to_add->triangles[tri_i].texturearray_i < 0);
        }
        
        if (to_add->triangles[tri_i].texturearray_i >= 0) {
            register_high_priority_if_unloaded(
                to_add->triangles[tri_i].texturearray_i,
                to_add->triangles[tri_i].texture_i);
        }
        
        to_add->triangles[tri_i].normal = get_ztriangle_normal(&to_add->triangles[tri_i]);
        normalize_zvertex(&to_add->triangles[tri_i].normal);
        
        // set the hitbox height, width, and depth
        set_zpolygon_hitbox(to_add);
    }
    
    for (
        uint32_t i = 0;
        i < zpolygons_to_render_size;
        i++)
    {
        if (zpolygons_to_render[i].deleted)
        {
            zpolygons_to_render[i] = *to_add;
            return;
        }
    }
    
    log_assert(zpolygons_to_render_size + 1 < ZPOLYGONS_TO_RENDER_ARRAYSIZE);
    zpolygons_to_render[zpolygons_to_render_size] = *to_add;
    zpolygons_to_render_size += 1;
}

void delete_zpolygon_object(const int32_t with_object_id)
{
    for (uint32_t i = 0; i < zpolygons_to_render_size; i++) {
        if (zpolygons_to_render[i].object_id == with_object_id)
        {
            zpolygons_to_render[i].deleted   = true;
            zpolygons_to_render[i].object_id = -1;
        }
    }
}

static uint32_t chars_till_next_space_or_slash(
    char * buffer)
{
    uint32_t i = 0;
    
    while (
        buffer[i] != '\n'
        && buffer[i] != ' '
        && buffer[i] != '/')
    {
        i++;
    }
    
    return i;
}

static uint32_t chars_till_next_nonspace(
    char * buffer)
{
    uint32_t i = 0;

    while (buffer[i] == ' ') {
        i++;
    }
    
    return i;
}

/*
Ducktaped together parser to read my .obj files from Blender
I export to 'legacy obj' in Blender like this:

Step 1:
File -> Export -> Wavefront (.obj) (Legacy)

Step 2:
Maximize the 'geometry' panel and check:
[x] Apply Modifiers
[x] Include UVs
[x] Write Materials (but don't use 'material groups' in the panel above)
[x] Triangulate faces
[x] Keep vertex order
Uncheck everything else
*/
void parse_obj(
    char * rawdata,
    uint64_t rawdata_size,
    const bool32_t flip_winding,
    zPolygon * recipient)
{
    parse_obj_expecting_materials(
        rawdata,
        rawdata_size,
        NULL,
        0,
        flip_winding,
        recipient);
}

void parse_obj_expecting_materials(
    char * rawdata,
    uint64_t rawdata_size,
    ExpectedObjMaterials * expected_materials,
    const uint32_t expected_materials_size,
    const bool32_t flip_winding,
    zPolygon * recipient)
{
    log_assert(rawdata != NULL);
    log_assert(rawdata_size > 0);
    
    construct_zpolygon(recipient);
    
    // TODO: think about buffer size
    // pass through buffer once to read all vertices 
    #define LOADING_OBJ_BUF_SIZE 16000
    zVertex * new_vertices = (zVertex *)malloc_from_managed(
        sizeof(zVertex) * LOADING_OBJ_BUF_SIZE);
    float uv_u[LOADING_OBJ_BUF_SIZE];
    float uv_v[LOADING_OBJ_BUF_SIZE];
    uint32_t new_uv_i = 0;
    
    uint32_t i = 0;
    uint32_t first_material_or_face_i = UINT32_MAX;
    uint32_t new_vertex_i = 0;
    
    while (i < rawdata_size) {
        // read the 1st character, which denominates the type
        // of information
        
        char dbg_newline[30];
        strcpy_capped(dbg_newline, 30, rawdata + i);
        uint32_t dbg_i = 0;
        while (dbg_i < 29 && dbg_newline[dbg_i] != '\n') {
            dbg_i++;
        }
        dbg_newline[dbg_i] = '\0';
        
        if (
            rawdata[i] == 'v' &&
            rawdata[i+1] == ' ')
        {
            // discard the 'v'
            i++;
            
            // read vertex data
            zVertex new_vertex;
            
            // skip the space(s) after the 'v'
            log_assert(rawdata[i] == ' ');
            i += chars_till_next_nonspace(rawdata + i);
            log_assert(rawdata[i] != ' ');
            
            // read vertex x
            new_vertex.x = string_to_float(rawdata + i);
            
            // discard vertex x
            i += chars_till_next_space_or_slash(
                rawdata + i);
            log_assert(rawdata[i] == ' ');
            
            // discard the spaces after vertex x
            i += chars_till_next_nonspace(rawdata + i);
            log_assert(rawdata[i] != ' ');
            
            // read vertex y
            new_vertex.y = string_to_float(rawdata + i);
            i += chars_till_next_space_or_slash(
                rawdata + i);
            log_assert(rawdata[i] == ' ');
            i += chars_till_next_nonspace(rawdata + i);
            log_assert(rawdata[i] != ' ');
            
            // read vertex z
            new_vertex.z = string_to_float(rawdata + i);
            i += chars_till_next_space_or_slash(
                rawdata + i);
            log_assert(rawdata[i] == '\n');
            i++;
            
            new_vertices[new_vertex_i] = new_vertex;
            log_assert(
                new_vertices[new_vertex_i].x == new_vertex.x);
            log_assert(
                new_vertices[new_vertex_i].y
                    == new_vertex.y);
            log_assert(
                new_vertices[new_vertex_i].z
                    == new_vertex.z);
            new_vertex_i++;
        } else if (
            rawdata[i] == 'v'
            && rawdata[i+1] == 't')
        {
            // discard the 'vt'
            i += 2;
            
            // skip the space(s) after the 'vt'
            log_assert(rawdata[i] == ' ');
            i += chars_till_next_nonspace(rawdata + i);
            log_assert(rawdata[i] != ' ');
            
            // read the u coordinate
            uv_u[new_uv_i] = string_to_float(rawdata + i);
            
            // discard the u coordinate
            i += chars_till_next_space_or_slash(
                rawdata + i);
            log_assert(rawdata[i] == ' ');
            
            // skip the space(s) after the u coord
            log_assert(rawdata[i] == ' ');
            i += chars_till_next_nonspace(rawdata + i);
            log_assert(rawdata[i] != ' ');
            
            // read the v coordinate
            uv_v[new_uv_i] = string_to_float(rawdata + i);
            
            new_uv_i += 1;
            
            // discard the v coordinate
            i += chars_till_next_space_or_slash(
                rawdata + i);
            log_assert(rawdata[i] == '\n');
            
            // discard the line break
            i++;
                        
        } else {
            if (
                rawdata[i] == 'f' ||
                (
                    rawdata[i] == 'u' &&
                    rawdata[i+1] == 's' &&
                    rawdata[i+2] == 'e' &&
                    rawdata[i+3] == 'm'))
            {
                if (i < first_material_or_face_i) {
                    first_material_or_face_i = i;
                }
            }
            
            if (rawdata[i] == 'f') {
                recipient->triangles_size += 1;
            }
            // skip until the next line break character 
            while (rawdata[i] != '\n' && rawdata[i] != '\0') {
                i++;
            }
            
            // skip the line break character
            i++;
        }
    }
    
    log_assert(recipient->triangles_size > 0);
    
    if (recipient->triangles_size >= POLYGON_TRIANGLES_SIZE) {
        char error_msg[100];
        strcpy_capped(error_msg, 100, "Error: POLYGON_TRIANGLES_SIZE was ");
        strcat_uint_capped(error_msg, 100, POLYGON_TRIANGLES_SIZE);
        strcat_capped(error_msg, 100, ", but recipient->triangles_size is ");
        strcat_uint_capped(error_msg, 100, recipient->triangles_size);
        log_dump_and_crash(error_msg);
        assert(0);
    }
    
    i = first_material_or_face_i;
    uint32_t new_triangle_i = 0;
    int32_t using_texturearray_i = -1;
    int32_t using_texture_i = -1;
    float using_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    while (i < rawdata_size) {
        if (rawdata[i] == 'u' &&
            rawdata[i+1] == 's' &&
            rawdata[i+2] == 'e' &&
            rawdata[i+3] == 'm')
        {
            uint32_t j = i + 1;
            while (
                rawdata[j] != '\n' &&
                rawdata[j] != '\0')
            {
                j++;
            }
            uint32_t line_size = j - i;
            
            if (expected_materials != NULL) {
                char usemtl_hint[line_size];
                char expected_mtl[line_size * 2];
                
                for (j = 0; j < (line_size); j++) {
                    usemtl_hint[j] = rawdata[i + j];
                }
                
                for (
                    uint32_t mtl_i = 0;
                    mtl_i < expected_materials_size;
                    mtl_i++)
                {
                    strcpy_capped(
                        expected_mtl,
                        line_size*2,
                        "usemtl ");
                    strcat_capped(
                        expected_mtl,
                        line_size*2,
                        expected_materials[mtl_i].material_name);
                    
                    if (
                        are_equal_strings_of_length(
                            expected_mtl,
                            usemtl_hint,
                            line_size))
                    {
                        using_texturearray_i =
                            expected_materials[mtl_i].texturearray_i;
                        using_texture_i =
                            expected_materials[mtl_i].texture_i;
                        using_color[0] =
                            expected_materials[mtl_i].rgba[0];
                        using_color[1] =
                            expected_materials[mtl_i].rgba[1];
                        using_color[2] =
                            expected_materials[mtl_i].rgba[2];
                        using_color[3] =
                            expected_materials[mtl_i].rgba[3];
                        break;
                    }
                }
            }
            
            // skip until the next line break character 
            while (rawdata[i] != '\n' && rawdata[i] != '\0') {
                i++;
            }
            // skip the line break character
            i++;
            
        } else if (rawdata[i] == 'f') {
            // discard the 'f'
            i++;
            log_assert(rawdata[i] == ' ');
            
            // skip the space(s) after the 'f'
            i += chars_till_next_nonspace(rawdata + i);
            log_assert(rawdata[i] != ' ');
            
            int32_t vertex_i_0 = string_to_int32(rawdata + i);
            i += chars_till_next_space_or_slash(
                rawdata + i);
            
            int32_t uv_coord_i_0 = 0;
            if (rawdata[i] == '/')
            {
                // skip the slash
                i++;
                uv_coord_i_0 = string_to_int32(rawdata + i);
                i += chars_till_next_space_or_slash(rawdata + i);
            }
            // skip any id's of normals
            if (rawdata[i] == '/') {
                i++;
                log_assert(rawdata[i] != ' ');
                i += chars_till_next_space_or_slash(rawdata + i);
            }
            
            log_assert(rawdata[i] == ' ');
            i += chars_till_next_nonspace(rawdata + i);
            log_assert(rawdata[i] != ' ');
            
            int32_t vertex_i_1 = string_to_int32(rawdata + i);
            i += chars_till_next_space_or_slash(
                rawdata + i);
            
            int32_t uv_coord_i_1 = 0;
            if (rawdata[i] == '/')
            {
                // skip the slash
                i++;
                uv_coord_i_1 =
                    string_to_int32(rawdata + i);
                i += chars_till_next_space_or_slash(
                    rawdata + i);
            }
            // skip any id's of normals
            if (rawdata[i] == '/') {
                i++;
                log_assert(rawdata[i] != ' ');
                i += chars_till_next_space_or_slash(rawdata + i);
            }
            
            log_assert(rawdata[i] == ' ');
            i += chars_till_next_nonspace(rawdata + i);
            log_assert(rawdata[i] != ' ');
            
            int32_t vertex_i_2 = string_to_int32(rawdata + i);
            i += chars_till_next_space_or_slash(
                rawdata + i);
            int32_t uv_coord_i_2 = 0;
            if (rawdata[i] == '/')
            {
                // skip the slash
                i++;
                uv_coord_i_2 =
                    string_to_int32(rawdata + i);
                i += chars_till_next_space_or_slash(
                    rawdata + i);
            }
            
            // skip any id's of normals
            if (rawdata[i] == '/') {
                i++;
                log_assert(rawdata[i] != ' ');
                while (rawdata[i] <= '9' && rawdata[i] >= '0') {
                    i++;
                }
            }
            
            while (rawdata[i] == ' ') { i++; }
            
            if (rawdata[i] != '\n') {
                int32_t vertex_i_3 = string_to_int32(rawdata + i);
                i += chars_till_next_space_or_slash(
                    rawdata + i);
                int32_t uv_coord_i_3 = 0;
                if (rawdata[i] == '/')
                {
                    // skip the slash
                    i++;
                    uv_coord_i_3 =
                        string_to_int32(rawdata + i);
                    i += chars_till_next_space_or_slash(
                        rawdata + i);
                }
                
                // skip any id's of normals
                if (rawdata[i] == '/') {
                    i++;
                    log_assert(rawdata[i] != ' ');
                    while (rawdata[i] <= '9' && rawdata[i] >= '0') {
                        i++;
                    }
                }
                
                // there were 2 triangles in this face
                // the 1st triangle will be added anyway later, but
                // we do need to add the extra 2nd triangle here
                zTriangle new_triangle;
                
                log_assert(vertex_i_0 != vertex_i_1);
                log_assert(vertex_i_0 != vertex_i_2);
                log_assert(vertex_i_0 > 0);
                log_assert(vertex_i_1 > 0);
                log_assert(vertex_i_2 > 0);
                log_assert(uv_coord_i_0 < LOADING_OBJ_BUF_SIZE);
                log_assert(uv_coord_i_1 < LOADING_OBJ_BUF_SIZE);
                log_assert(uv_coord_i_2 < LOADING_OBJ_BUF_SIZE);
                
                uint32_t target_vertex_0 = 0;
                uint32_t target_vertex_1 = 1;
                uint32_t target_vertex_2 = 2;
                
                new_triangle.vertices[target_vertex_0] =
                    new_vertices[vertex_i_0 - 1];
                new_triangle.vertices[target_vertex_1] =
                    new_vertices[vertex_i_2 - 1];
                new_triangle.vertices[target_vertex_2] =
                    new_vertices[vertex_i_3 - 1];
                
                if (
                    uv_coord_i_0 > 0 &&
                    uv_coord_i_1 > 0 &&
                    uv_coord_i_2 > 0)
                {
                    new_triangle.vertices[target_vertex_0].uv[0] =
                    uv_u[uv_coord_i_0 - 1];
                    new_triangle.vertices[target_vertex_0].uv[1] =
                    uv_v[uv_coord_i_0 - 1];
                    new_triangle.vertices[target_vertex_1].uv[0] =
                    uv_u[uv_coord_i_2 - 1];
                    new_triangle.vertices[target_vertex_1].uv[1] =
                    uv_v[uv_coord_i_2 - 1];
                    new_triangle.vertices[target_vertex_2].uv[0] =
                    uv_u[uv_coord_i_3 - 1];
                    new_triangle.vertices[target_vertex_2].uv[1] =
                    uv_v[uv_coord_i_3 - 1];
                }
                
                new_triangle.color[0] = using_color[0];
                new_triangle.color[1] = using_color[1];
                new_triangle.color[2] = using_color[2];
                new_triangle.color[3] = using_color[3];
                
                new_triangle.texturearray_i = using_texturearray_i;
                new_triangle.texture_i = using_texture_i;
                
                recipient->triangles_size += 1;
                log_assert(new_triangle_i < POLYGON_TRIANGLES_SIZE);
                
                recipient->triangles[new_triangle_i] = new_triangle;
                new_triangle_i++;
            } else {
                // there was only 1 triangle
            }
            
            // if you get here there was only 1 triangle OR
            // there were 2 triangles and you already did the other one
            zTriangle new_triangle;
            
            log_assert(vertex_i_0 != vertex_i_1);
            log_assert(vertex_i_0 != vertex_i_2);
            log_assert(vertex_i_0 > 0);
            log_assert(vertex_i_1 > 0);
            log_assert(vertex_i_2 > 0);
            log_assert(uv_coord_i_0 < LOADING_OBJ_BUF_SIZE);
            log_assert(uv_coord_i_1 < LOADING_OBJ_BUF_SIZE);
            log_assert(uv_coord_i_2 < LOADING_OBJ_BUF_SIZE);
            
            uint32_t target_vertex_0 = flip_winding ? 2 : 0;
            uint32_t target_vertex_1 = 1;
            uint32_t target_vertex_2 = flip_winding ? 0 : 2;
            
            new_triangle.vertices[target_vertex_0] =
                new_vertices[vertex_i_0 - 1];
            new_triangle.vertices[target_vertex_1] =
                new_vertices[vertex_i_1 - 1];
            new_triangle.vertices[target_vertex_2] =
                new_vertices[vertex_i_2 - 1];
            
            if (
                uv_coord_i_0 > 0 &&
                uv_coord_i_1 > 0 &&
                uv_coord_i_2 > 0)
            {
                new_triangle.vertices[target_vertex_0].uv[0] =
                uv_u[uv_coord_i_0 - 1];
                new_triangle.vertices[target_vertex_0].uv[1] =
                uv_v[uv_coord_i_0 - 1];
                new_triangle.vertices[target_vertex_1].uv[0] =
                uv_u[uv_coord_i_1 - 1];
                new_triangle.vertices[target_vertex_1].uv[1] =
                uv_v[uv_coord_i_1 - 1];
                new_triangle.vertices[target_vertex_2].uv[0] =
                uv_u[uv_coord_i_2 - 1];
                new_triangle.vertices[target_vertex_2].uv[1] =
                uv_v[uv_coord_i_2 - 1];
            }
            
            new_triangle.color[0] = using_color[0];
            new_triangle.color[1] = using_color[1];
            new_triangle.color[2] = using_color[2];
            new_triangle.color[3] = using_color[3];
            
            new_triangle.texturearray_i = using_texturearray_i;
            new_triangle.texture_i = using_texture_i;
            
            if (new_triangle_i >= POLYGON_TRIANGLES_SIZE) {
                char err_txt[300];
                strcpy_capped(err_txt, 300, "Tried to add new_triangle_i: ");
                strcat_uint_capped(err_txt, 300, new_triangle_i);
                strcat_capped(err_txt, 300, ", but ZPOLYGONS_TO_RENDER_ARRAYSIZE was: ");
                strcat_int_capped(err_txt, 300, ZPOLYGONS_TO_RENDER_ARRAYSIZE);
                strcat_capped(err_txt, 300, "\n");
                log_dump_and_crash(err_txt);
            }
            recipient->triangles[new_triangle_i] = new_triangle;
            new_triangle_i++;
            
            log_assert(rawdata[i] == '\n');
            i++;
            
        } else {
            // skip until the next line break character 
            while (rawdata[i] != '\n' && rawdata[i] != '\0') {
                i++;
            }
            
            // skip the line break character
            i++;
        }
    }
    
    free_from_managed((uint8_t *)new_vertices);
    
    if (recipient->triangles_size >= POLYGON_TRIANGLES_SIZE) {
        char error_msg[100];
        strcpy_capped(error_msg, 100, "Error: POLYGON_TRIANGLES_SIZE was ");
        strcat_uint_capped(error_msg, 100, POLYGON_TRIANGLES_SIZE);
        strcat_capped(error_msg, 100, ", but recipient->triangles_size is ");
        strcat_uint_capped(error_msg, 100, recipient->triangles_size);
        strcat_capped(error_msg, 100, "\n");
        log_dump_and_crash(error_msg);
        assert(0);
    }
}

void zpolygon_scale_to_width_given_z(
    zPolygon * to_scale,
    const float new_width,
    const float when_observed_at_z)
{
    float largest_width = 0.0f;
    for (uint32_t i = 0; i < to_scale->triangles_size; i++) {
        for (uint32_t j = 0; j < 3; j++)
        {
            float width =
                ((to_scale->triangles[i].vertices[j].x < 0) *  (to_scale->triangles[i].vertices[j].x * -1)) +
                ((to_scale->triangles[i].vertices[j].x >= 0) *  (to_scale->triangles[i].vertices[j].x)); 
            if (width > largest_width)
            {
                largest_width = width;
            }
        }
    }
    
    float target_width = new_width / when_observed_at_z;
    
    float scale_factor = target_width / largest_width;
    
    for (uint32_t i = 0; i < to_scale->triangles_size; i++) {
        for (uint32_t j = 0; j < 3; j++)
        {
            to_scale->triangles[i].vertices[j].x *= scale_factor;
            to_scale->triangles[i].vertices[j].y *= scale_factor;
            to_scale->triangles[i].vertices[j].z *= scale_factor;
        }
    }
}

void scale_zpolygon(
    zPolygon * to_scale,
    const float new_size)
{
    log_assert(to_scale != NULL);
    if (to_scale == NULL) { return; }
        
    float highest_ascent = 0.0f;
    float lowest_descent = 0.0f;
    
    for (
        uint32_t i = 0;
        i < to_scale->triangles_size;
        i++)
    {
        for (
            uint32_t j = 0;
            j < 3;
            j++)
        {
            if (
                to_scale->triangles[i].vertices[j].y > highest_ascent)
            {
                highest_ascent = to_scale->triangles[i].vertices[j].y;
            }
            
            if (
                to_scale->triangles[i].vertices[j].y < lowest_descent)
            {
                lowest_descent = to_scale->triangles[i].vertices[j].y;
            }
        }
    }
    log_assert(highest_ascent > lowest_descent);
    
    float scale_factor = new_size / (highest_ascent - lowest_descent);
    
    for (uint32_t i = 0; i < to_scale->triangles_size; i++) {
        for (uint32_t j = 0; j < 3; j++)
        {
            to_scale->triangles[i].vertices[j].x *= scale_factor;
            to_scale->triangles[i].vertices[j].y *= scale_factor;
            to_scale->triangles[i].vertices[j].z *= scale_factor;
        }
    }
}

void center_zpolygon_offsets(
    zPolygon * to_center)
{
    log_assert(to_center != NULL);
    if (to_center == NULL) { return; }
    
    float smallest_y = FLOAT32_MAX;
    float largest_y = FLOAT32_MIN;
    float smallest_x = FLOAT32_MAX;
    float largest_x = FLOAT32_MIN;
    float smallest_z = FLOAT32_MAX;
    float largest_z = FLOAT32_MIN;
    
    for (uint32_t i = 0; i < to_center->triangles_size; i++) {
        for (uint32_t j = 0; j < 3; j++)
        {
            float x = to_center->triangles[i].vertices[j].x;
            float y = to_center->triangles[i].vertices[j].y;
            float z = to_center->triangles[i].vertices[j].z;
            
            if (y > largest_y)  { largest_y = y;  }
            if (y < smallest_y) { smallest_y = y; }
            if (x > largest_x)  { largest_x = x;  }
            if (x < smallest_x) { smallest_x = x; }
            if (z > largest_z)  { largest_z = z;  }
            if (z < smallest_z) { smallest_z = z; }
        }
    }
    log_assert(largest_y > 0.0f);
    log_assert(largest_y > smallest_y);
    log_assert(largest_x > smallest_x);
    log_assert(largest_z > smallest_z);
    
    float x_delta = (smallest_x + largest_x) / 2.0f;
    float y_delta = (smallest_y + largest_y) / 2.0f;
    float z_delta = (smallest_z + largest_z) / 2.0f;
    
    for (uint32_t i = 0; i < to_center->triangles_size; i++) {
        for (uint32_t j = 0; j < 3; j++)
        {
            to_center->triangles[i].vertices[j].x -= x_delta;
            to_center->triangles[i].vertices[j].y -= y_delta;
            to_center->triangles[i].vertices[j].z -= z_delta;
        }
    }
}

void construct_zpolygon(zPolygon * to_construct) {
    to_construct->object_id = -1;
    to_construct->touchable_id = -1;
    to_construct->triangles_size = 0;
    to_construct->x = 0.0f;
    to_construct->y = 0.0f;
    to_construct->z = 1.0f;
    to_construct->x_angle = 0.0f;
    to_construct->y_angle = 0.0f;
    to_construct->z_angle = 0.0f;
    to_construct->scale_factor = 1.0f;
    to_construct->ignore_lighting = false;
    to_construct->ignore_camera = false;
    to_construct->visible = true;
    to_construct->deleted = false;
    to_construct->rgb_bonus[0] = 0.0f;
    to_construct->rgb_bonus[1] = 0.0f;
    to_construct->rgb_bonus[2] = 0.0f;
}

zTriangle
x_rotate_ztriangle(
    const zTriangle * input,
    const float angle)
{
    zTriangle return_value = *input;
    
    if (angle == 0.0f) {
        return return_value;
    }
    
    for (
        uint32_t i = 0;
        i < 3;
        i++)
    {
        return_value.vertices[i] = x_rotate_zvertex(
            &return_value.vertices[i],
            angle);
        
        return_value.normal = x_rotate_zvertex(
            &return_value.normal,
            angle);
    }
    
    return return_value;
}


zTriangle
z_rotate_ztriangle(
    const zTriangle * input,
    const float angle)
{
    zTriangle return_value = *input;
    
    if (angle == 0.0f) {
        return return_value;
    }
    
    for (
        uint32_t i = 0;
        i < 3;
        i++)
    {
        return_value.vertices[i] = z_rotate_zvertex(
            &return_value.vertices[i],
            angle);
        
        return_value.normal = z_rotate_zvertex(
            &return_value.normal,
            angle);
    }
    
    return return_value;
}

zTriangle
y_rotate_ztriangle(
    const zTriangle * input,
    const float angle)
{
    zTriangle return_value = *input;
    
    if (angle == 0.0f) {
        return return_value;
    }
    
    for (
        uint32_t i = 0;
        i < 3;
        i++)
    {
        return_value.vertices[i] = y_rotate_zvertex(
            &return_value.vertices[i],
            angle);
        
        return_value.normal = y_rotate_zvertex(
            &return_value.normal,
            angle);
    }
    
    return return_value;
}

zTriangle translate_ztriangle(
    const zTriangle * input,
    const float by_x,
    const float by_y,
    const float by_z)
{
    zTriangle return_value = *input;
    
    for (uint32_t i = 0; i < 3; i++) {
        return_value.vertices[i].x += by_x;
        return_value.vertices[i].y += by_y;
        return_value.vertices[i].z += by_z;
    }
    
    return return_value;
}

float get_avg_z(
    const zTriangle * of_triangle)
{
    return (
        of_triangle->vertices[0].z +
        of_triangle->vertices[1].z +
        of_triangle->vertices[2].z) / 3.0f;
}

int sorter_cmpr_lowest_z(
    const void * a,
    const void * b)
{
    return get_avg_z((zTriangle *)a) < get_avg_z((zTriangle *)b) ? -1 : 1;
}

float get_distance(
    const zVertex p1,
    const zVertex p2)
{
    return sqrtf(
        ((p1.x - p2.x) * (p1.x - p2.x))
        + ((p1.y - p2.y) * (p1.y - p2.y))
        + ((p1.z - p2.z) * (p1.z - p2.z)));
}

float distance_to_ztriangle(
    const zVertex p1,
    const zTriangle p2)
{
    return (
        get_distance(p1, p2.vertices[0]) +
        get_distance(p1, p2.vertices[1]) +
        get_distance(p1, p2.vertices[2])) / 3.0f;
}

zVertex get_ztriangle_normal(
    const zTriangle * input)
{
    uint32_t vertex_0 = 0;
    uint32_t vertex_1 = 1;
    uint32_t vertex_2 = 2;
    
    zVertex normal;
    zVertex vector1;
    zVertex vector2;
    
    vector1.x = input->vertices[vertex_1].x - input->vertices[vertex_0].x;
    vector1.y = input->vertices[vertex_1].y - input->vertices[vertex_0].y;
    vector1.z = input->vertices[vertex_1].z - input->vertices[vertex_0].z;
    
    vector2.x = input->vertices[vertex_2].x - input->vertices[vertex_0].x;
    vector2.y = input->vertices[vertex_2].y - input->vertices[vertex_0].y;
    vector2.z = input->vertices[vertex_2].z - input->vertices[vertex_0].z;
    
    normal.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
    normal.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
    normal.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);
    
    return normal;
}

float dot_of_zvertices(
    const zVertex * a,
    const zVertex * b)
{
    return
        (a->x * b->x) +
        (a->y * b->y) +
        (a->z * b->z);
}

zVertex crossproduct_of_zvertices(
    const zVertex * a,
    const zVertex * b)
{
    /*
    cx = aybz − azby
    cy = azbx − axbz
    cz = axby − aybx
    */
    zVertex result;
    
    result.x = (a->y * b->z) - (a->z * b->y);
    result.y = (a->z * b->x) - (a->x * b->z);
    result.z = (a->x * b->y) - (a->y * b->x);
    
    return result;
}

void zcamera_move_forward(
    zCamera * to_move,
    const float distance)
{
    // pick a point that would be in front of the camera
    // if it was not angled in any way, and if it was at
    // the origin
    zVertex forward_if_camera_was_unrotated_at_origin;
    forward_if_camera_was_unrotated_at_origin.x = 0.0f;
    forward_if_camera_was_unrotated_at_origin.y = 0.0f;
    forward_if_camera_was_unrotated_at_origin.z = distance;
    
    zVertex x_rotated = x_rotate_zvertex(
        &forward_if_camera_was_unrotated_at_origin,
        camera.x_angle);
    zVertex y_rotated = y_rotate_zvertex(
        &x_rotated,
        camera.y_angle);
    zVertex final = z_rotate_zvertex(
        &y_rotated,
        camera.z_angle);
    
    // add to the camera's current position
    to_move->x += final.x;
    to_move->y += final.y;
    to_move->z += final.z;
}

//bool32_t ray_intersects_triangle(
//    const zVertex * ray_origin,
//    const zVertex * ray_direction,
//    const zTriangle * triangle,
//    zVertex * recipient_hit_point)
//{
//    /*
//    Reminder: The plane offset is named 'D' in this explanation:
//    "...and D is the distance from the origin (0, 0, 0) to the
//    plane (if we trace a line from the origin to the plane,
//    parallel to the plane's normal)."
//    "..we know the plane's normal and that the three triangle's
//    vertices (V0, V1, V2) lie in the plane. It is, therefore,
//    possible to compute  D. Any of the three vertices can be
//    chosen. Let's choose V0:
//    float D = -dotProduct(N, v0);"
//    (source https://www.scratchapixel.com
//    */
//    float plane_offset = -1.0f * dot_of_zvertices(
//        &triangle->normal,
//        &triangle->vertices[0]);
//    
//    /*
//    We also know that point P is the intersection point of the ray, and the 
//    point lies in the plane. Consequently, we can substitute (x, y, z)
//    for P or O + tR that P is equal to and solve for t:
//    
//    float t = - (dot(N, orig) + D) / dot(N, dir);
//    */
//    float denominator = dot_of_zvertices(
//        &triangle->normal,
//        ray_direction);
//    if (denominator < 0.0001f && denominator > -0.0001f) {
//        // the ray doesn't intersect with the triangle's plane,
//        // I think this is always because the ray travels in parallel with the
//        // triangle
//        return false;
//    }
//    
//    float t =
//        (-1.0f * (
//            dot_of_zvertices(&triangle->normal, ray_origin) +
//                plane_offset)) /
//            denominator;
//    
//    // if t is < 0, the triangle's plane must be behind us which counts as
//    // a miss
//    if (t <= 0.0f) {
//        return false;
//    }
//    
//    // We now have computed t, which we can use to calculate the position of P:
//    // Vec3f Phit = orig + t * dir;
//    recipient_hit_point->x = ray_origin->x + (t * ray_direction->x);
//    recipient_hit_point->y = ray_origin->y + (t * ray_direction->y);
//    recipient_hit_point->z = ray_origin->z + (t * ray_direction->z);
//    
//    /*
//    Now that we have found the point P, which is the point where the ray and
//    the plane intersect, we still have to find out if P is inside the triangle
//    (in which case the ray intersects the triangle) or if P is outside (in
//    which case the rays misses the triangle)
//    
//    to find out if P is inside the triangle, we can test if the dot product of
//    the vector along the edge and the vector defined by the first vertex of the
//    tested edge and P is positive (meaning P is on the left side of the edge).
//    If P is on the left of all three edges, then P is inside the triangle.
//    
//    pseudocode:
//    Vec3f edge0 = v1 - v0;
//    Vec3f edge1 = v2 - v1;
//    Vec3f edge2 = v0 - v2;
//    Vec3f C0 = P - v0;
//    Vec3f C1 = P - v1;
//    Vec3f C2 = P - v2;
//    if (
//        dotProduct(N, crossProduct(edge0, C0)) > 0 && 
//        dotProduct(N, crossProduct(edge1, C1)) > 0 &&
//        dotProduct(N, crossProduct(edge2, C2)) > 0)
//    {
//        return true; // P is inside the triangle
//    }
//    */
//    zVertex edge0;
//    edge0.x = triangle->vertices[1].x - triangle->vertices[0].x;
//    edge0.y = triangle->vertices[1].y - triangle->vertices[0].y;
//    edge0.z = triangle->vertices[1].z - triangle->vertices[0].z;
//    
//    zVertex edge1;
//    edge1.x = triangle->vertices[2].x - triangle->vertices[1].x;
//    edge1.y = triangle->vertices[2].y - triangle->vertices[1].y;
//    edge1.z = triangle->vertices[2].z - triangle->vertices[1].z;
//    
//    zVertex edge2;
//    edge2.x = triangle->vertices[0].x - triangle->vertices[2].x;
//    edge2.y = triangle->vertices[0].y - triangle->vertices[2].y;
//    edge2.z = triangle->vertices[0].z - triangle->vertices[2].z;
//    
//    zVertex C0;
//    C0.x = recipient_hit_point->x - triangle->vertices[0].x;
//    C0.y = recipient_hit_point->y - triangle->vertices[0].y;
//    C0.z = recipient_hit_point->z - triangle->vertices[0].z;
//    
//    zVertex C1;
//    C1.x = recipient_hit_point->x - triangle->vertices[1].x;
//    C1.y = recipient_hit_point->y - triangle->vertices[1].y;
//    C1.z = recipient_hit_point->z - triangle->vertices[1].z;
//    
//    zVertex C2;
//    C2.x = recipient_hit_point->x - triangle->vertices[2].x;
//    C2.y = recipient_hit_point->y - triangle->vertices[2].y;
//    C2.z = recipient_hit_point->z - triangle->vertices[2].z;
//    
//    zVertex cross0 = crossproduct_of_zvertices(&edge0, &C0);
//    zVertex cross1 = crossproduct_of_zvertices(&edge1, &C1);
//    zVertex cross2 = crossproduct_of_zvertices(&edge2, &C2);
//    
//    if (
//        dot_of_zvertices(
//            &triangle->normal,
//            &cross0) > 0.0f &&
//        dot_of_zvertices(
//            &triangle->normal,
//            &cross1) > 0.0f &&
//        dot_of_zvertices(
//            &triangle->normal,
//            &cross2) > 0.0f)
//    {        
//        return true;
//    }
//    
//    return false;
//}

//bool32_t ray_intersects_zpolygon_triangles(
//    const zVertex * ray_origin,
//    const zVertex * ray_direction,
//    const zPolygon * mesh,
//    zVertex * recipient_hit_point)
//{
//    for (
//        uint32_t tri_i = 0;
//        tri_i < mesh->triangles_size;
//        tri_i++)
//    {
//        zTriangle triangle = mesh->triangles[tri_i];
//        
//        x_rotate_ztriangle(&triangle, mesh->x_angle);
//        y_rotate_ztriangle(&triangle, mesh->y_angle);
//        z_rotate_ztriangle(&triangle, mesh->z_angle);
//        
//        for (uint32_t m = 0; m < 3; m++) {
//            triangle.vertices[m].x += mesh->x;
//            triangle.vertices[m].y += mesh->y;
//            triangle.vertices[m].z += mesh->z;
//        }
//        
//        float dist_to_raypos = get_distance(
//            *ray_origin,
//            triangle.vertices[0]);
//        
//        float lowest_hit_dist = FLOAT32_MAX;
//        
//        if (dist_to_raypos < lowest_hit_dist) {
//            
//            bool32_t ray_intersects = ray_intersects_triangle(
//                /* const zVertex * ray_origin: */
//                    ray_origin,
//                /* const zVertex * ray_direction: */
//                    ray_direction,
//                /* const zTriangle * triangle: */
//                    &triangle,
//                /* collision_point: */
//                    recipient_hit_point);
//            
//            if (ray_intersects) {
//                lowest_hit_dist = dist_to_raypos;
//                return true; // no need to check other triangles in this zpoly
//            }
//        }
//    }
//    
//    return false;
//}

bool32_t ray_intersects_zpolygon_hitbox(
    const zVertex * ray_origin,
    const zVertex * ray_direction,
    const zPolygon * mesh,
    zVertex * recipient_hit_point)
{
    /*
    Reminder: The plane offset is named 'D' in this explanation:
    "...and D is the distance from the origin (0, 0, 0) to the
    plane (if we trace a line from the origin to the plane,
    parallel to the plane's normal)."
    "..we know the plane's normal and that the three triangle's
    vertices (V0, V1, V2) lie in the plane. It is, therefore,
    possible to compute  D. Any of the three vertices can be
    chosen. Let's choose V0:
    float D = -dotProduct(N, v0);"
    (source https://www.scratchapixel.com
    */
    zVertex     mesh_center;
    mesh_center.x = mesh->x;
    mesh_center.y = mesh->y;
    mesh_center.z = mesh->z;
    
    zVertex plane_normals[6];
    float plane_offsets[6];
    float t_values[6];
    bool32_t hit_plane[6];
    zVertex hit_points[6];
    
    // These are normals, but they're also offsets to turn the mesh center
    // into the plane for that box face
    // Plane facing towards camera
    plane_normals[0].x =                     0.0f;
    plane_normals[0].y =                     0.0f;
    plane_normals[0].z =  -mesh->hitbox_depth / 2;
    // Plane facing away from camera
    plane_normals[1].x =                     0.0f;
    plane_normals[1].y =                     0.0f;
    plane_normals[1].z =   mesh->hitbox_depth / 2;
    // Plane facing up
    plane_normals[2].x =                     0.0f;
    plane_normals[2].y =  mesh->hitbox_height / 2;
    plane_normals[2].z =                     0.0f;
    // Plane facing down
    plane_normals[3].x =                     0.0f;
    plane_normals[3].y = -mesh->hitbox_height / 2;
    plane_normals[3].z =                     0.0f;
    // Plane facing left
    plane_normals[4].x =  -mesh->hitbox_width / 2;
    plane_normals[4].y =                     0.0f;
    plane_normals[4].z =                     0.0f;
    // Plane facing right
    plane_normals[5].x =   mesh->hitbox_width / 2;
    plane_normals[5].y =                     0.0f;
    plane_normals[5].z =                     0.0f;
    
    #define PLANES_TO_CHECK 6
    // NaN checks
    for (uint32_t p = 0; p < PLANES_TO_CHECK; p++) {
        log_assert(plane_normals[p].x == plane_normals[p].x);
        log_assert(plane_normals[p].y == plane_normals[p].y);
        log_assert(plane_normals[p].z == plane_normals[p].z);
        
        plane_normals[p]    = x_rotate_zvertex(
            &plane_normals[p], mesh->x_angle);
        plane_normals[p]    = y_rotate_zvertex(
            &plane_normals[p], mesh->y_angle);
        plane_normals[p]    = z_rotate_zvertex(
            &plane_normals[p], mesh->z_angle);
    }
    
    for (uint32_t p = 0; p < PLANES_TO_CHECK; p++) {
        
        hit_plane[p] = false;
        
        // before normalizing, offset to find the plane we're checking
        zVertex current_plane_origin;
        current_plane_origin.x  = mesh->x;
        current_plane_origin.y  = mesh->y;
        current_plane_origin.z  = mesh->z;
        current_plane_origin.x += plane_normals[p].x;
        current_plane_origin.y += plane_normals[p].y;
        current_plane_origin.z += plane_normals[p].z;
        
        // now we can normalize the offsets and use them as our normal value
        zVertex normalized_plane_normal = plane_normals[p];
        normalize_zvertex(&normalized_plane_normal);
        
        /*
        A plane is defined as:
        A*x + B*x + C*z + D = 0
        or:
        D = -(A*x + B*y + C*z)
        
        and since the normal is pointing to the plane, its xyz values are
        literally the same as ABC
        
        for xyz we can use any point inside the plane, so let's use the plane
        origin
        
        Distance from Origin
        "If the normal vector is normalized (unit length), then the constant
        term of the plane equation, d becomes the distance from the origin."
        */
        plane_offsets[p] = -dot_of_zvertices(
            &normalized_plane_normal,
            &current_plane_origin);
        
        /*
        We also know that point P is the intersection point of the ray, and the 
        point lies in the plane. Consequently, we can substitute (x, y, z)
        for P or O + tR that P is equal to and solve for t
        
        after shuffling around you can get:
        t = (p0 - lo) dot N / l dot N
        
        where l is the ray (lo is ray origin, l is ray direction) and
        N is the plane's normal and p0 is the plane origin
        
        or from another source on triangle intersection (I don't understand
        the difference because the triangle solution uses a plane first):
        float t = -(dot(N, orig) + D) / dot(N, dir);
        
        these give the same results so that part must be accurate!
        
          ___
        q(t.t)p
          ( )
          d.b
        */
        float denominator = dot_of_zvertices(
            &normalized_plane_normal,
            ray_direction);
        
        if (
            denominator <  0.0001f &&
            denominator > -0.0001f)
        {
            // the ray doesn't intersect with the triangle's plane,
            // I think this is always because the ray travels in parallel with the
            // triangle
            continue;
        }
        
        zVertex plane_offset_minus_ray_origin = current_plane_origin;
        plane_offset_minus_ray_origin.x -= ray_origin->x;
        plane_offset_minus_ray_origin.y -= ray_origin->y;
        plane_offset_minus_ray_origin.z -= ray_origin->z;
        
        t_values[p] = -(
            dot_of_zvertices(&normalized_plane_normal, ray_origin) +
                plane_offsets[p]);
        t_values[p] /= denominator;
        
        // This should give about the same result:
        float t_values_alternative = dot_of_zvertices(
            &normalized_plane_normal,
            &plane_offset_minus_ray_origin) /
                denominator;
        
        float diff = t_values[p] - t_values_alternative;
        log_assert(diff <  0.1f);
        log_assert(diff > -0.1f);
        // end of debug check
        
        // if t is < 0, the triangle's plane must be behind us which counts as
        // a miss
        if (t_values[p] < 0.0f) {
            continue;
        }
        
        // We now have computed t, which we can use to calculate the position of P:
        // Vec3f Phit = orig + t * dir;
        zVertex raw_collision_point;
        raw_collision_point.x = ray_origin->x +
            (t_values[p] * ray_direction->x);
        raw_collision_point.y = ray_origin->y +
            (t_values[p] * ray_direction->y); 
        raw_collision_point.z = ray_origin->z +
            (t_values[p] * ray_direction->z);
        
        hit_points[p] = raw_collision_point;
        
        zVertex center_to_hit_ray;
        center_to_hit_ray.x = raw_collision_point.x - mesh->x;
        center_to_hit_ray.y = raw_collision_point.y - mesh->y;
        center_to_hit_ray.z = raw_collision_point.z - mesh->z;
        
        zVertex reverse_rotated_center_to_hit = center_to_hit_ray;        
        reverse_rotated_center_to_hit =
            x_rotate_zvertex(&reverse_rotated_center_to_hit, -mesh->x_angle);
        reverse_rotated_center_to_hit =
            y_rotate_zvertex(&reverse_rotated_center_to_hit, -mesh->y_angle);
        reverse_rotated_center_to_hit =
            z_rotate_zvertex(&reverse_rotated_center_to_hit, -mesh->z_angle);
        
        // Now I just want to check if this hit is within the zpolygon's
        // hitbox
        // it's now an 'axis aligned' hitbox (since we rotated the collision
        // point and not the hitbox itself), so we can just check the bounds
        float tolerance = 0.03f;
        if (
            (reverse_rotated_center_to_hit.x - tolerance) <= 
                 ( mesh->hitbox_width  / 2 ) &&
            (reverse_rotated_center_to_hit.x + tolerance) >=
                -( mesh->hitbox_width  / 2 ) &&
            (reverse_rotated_center_to_hit.y - tolerance) <=
                 ( mesh->hitbox_height / 2 ) &&
            (reverse_rotated_center_to_hit.y + tolerance) >=
                -( mesh->hitbox_height / 2 ) &&
            (reverse_rotated_center_to_hit.z - tolerance) <=
                 ( mesh->hitbox_depth  / 2 ) &&
            (reverse_rotated_center_to_hit.z + tolerance) >=
                -( mesh->hitbox_depth  / 2 ))
        {
            hit_plane[p] = true;
        }
    }
    
    bool32_t return_value = false;
    float closest_t = FLOAT32_MAX;
    
    for (uint32_t p = 0; p < PLANES_TO_CHECK; p++) {
        if (
            hit_plane[p] &&
            t_values[p] < closest_t)
        {
            return_value = true;
            *recipient_hit_point = hit_points[p];
            closest_t = t_values[p];
        }
    }
    
    return return_value;
}

void construct_quad(
    const float left_x,
    const float bottom_y,
    const float z,
    const float width,
    const float height,
    zPolygon * recipient)
{
    log_assert(z > 0.0f);
    
    construct_zpolygon(recipient);
    
    recipient->triangles_size = 2;
    
    const float mid_x           = left_x + (width  / 2);
    const float mid_y           = bottom_y  + (height / 2);
    
    const float left_vertex     = (left_x - mid_x );
    const float right_vertex    = (mid_x  - left_x);
    const float top_vertex      = (mid_y  - bottom_y );
    const float bottom_vertex   = (bottom_y - mid_y );
    
    const float left_uv_coord   = 0.0f;
    const float right_uv_coord  = 1.0f;
    const float bottom_uv_coord = 1.0f;
    const float top_uv_coord    = 0.0f;
    
    recipient->x = mid_x;
    recipient->y = mid_y;
    recipient->z = z;
    recipient->visible = true;
    
    // **
    // top & left triangle
    // **
    // top left vertex
    recipient->triangles[0].vertices[0].x     = left_vertex;
    recipient->triangles[0].vertices[0].y     = top_vertex;
    recipient->triangles[0].vertices[0].z     = 0.0f;
    recipient->triangles[0].vertices[0].uv[0] = left_uv_coord;
    recipient->triangles[0].vertices[0].uv[1] = top_uv_coord;
    // top right vertex
    recipient->triangles[0].vertices[1].x     = right_vertex;
    recipient->triangles[0].vertices[1].y     = top_vertex;
    recipient->triangles[0].vertices[1].z     = 0.0f;
    recipient->triangles[0].vertices[1].uv[0] = right_uv_coord;
    recipient->triangles[0].vertices[1].uv[1] = top_uv_coord;
    // bottom left vertex
    recipient->triangles[0].vertices[2].x     = left_vertex;
    recipient->triangles[0].vertices[2].y     = bottom_vertex;
    recipient->triangles[0].vertices[2].z     = 0.0f;
    recipient->triangles[0].vertices[2].uv[0] = left_uv_coord;
    recipient->triangles[0].vertices[2].uv[1] = bottom_uv_coord;
    
    recipient->triangles[0].normal.x       = 0.0f;
    recipient->triangles[0].normal.y       = 0.0f;
    recipient->triangles[0].normal.z       = 1.0f;
    recipient->triangles[0].texturearray_i = -1;
    recipient->triangles[0].texture_i      = -1;
    recipient->triangles[0].color[0]       = 1.0f;
    recipient->triangles[0].color[1]       = 1.0f;
    recipient->triangles[0].color[2]       = 1.0f;
    recipient->triangles[0].color[3]       = 1.0f;
    
    // **
    // right & bottom triangle
    // **
    // top right vertex
    recipient->triangles[1].vertices[0].x     = right_vertex;
    recipient->triangles[1].vertices[0].y     = top_vertex;
    recipient->triangles[1].vertices[0].z     = 0.0f;
    recipient->triangles[1].vertices[0].uv[0] = right_uv_coord;
    recipient->triangles[1].vertices[0].uv[1] = top_uv_coord;
    // bottom right vertex
    recipient->triangles[1].vertices[1].x     = right_vertex;
    recipient->triangles[1].vertices[1].y     = bottom_vertex;
    recipient->triangles[1].vertices[1].z     = 0.0f;
    recipient->triangles[1].vertices[1].uv[0] = right_uv_coord;
    recipient->triangles[1].vertices[1].uv[1] = bottom_uv_coord;
    // bottom left vertex
    recipient->triangles[1].vertices[2].x     = left_vertex;
    recipient->triangles[1].vertices[2].y     = bottom_vertex;
    recipient->triangles[1].vertices[2].z     = 0.0f;
    recipient->triangles[1].vertices[2].uv[0] = left_uv_coord;
    recipient->triangles[1].vertices[2].uv[1] = bottom_uv_coord;
    
    recipient->triangles[1].normal.x       = 0.0f;
    recipient->triangles[1].normal.y       = 0.0f;
    recipient->triangles[1].normal.z       = 1.0f;
    recipient->triangles[1].texturearray_i = -1;
    recipient->triangles[1].texture_i      = -1;
    recipient->triangles[1].color[0]       = 1.0f;
    recipient->triangles[1].color[1]       = 1.0f;
    recipient->triangles[1].color[2]       = 1.0f;
    recipient->triangles[1].color[3]       = 1.0f;
}

void construct_quad_around(
    const float mid_x,
    const float mid_y,
    const float z,
    const float width,
    const float height,
    zPolygon * recipient)
{
    log_assert(z > 0.0f);
    
    construct_zpolygon(recipient);
    
    recipient->triangles_size = 2;
    
    const float projected_mid_x = mid_x;
    const float projected_mid_y = mid_y;
    
    const float left_vertex     = -(width  / 2);
    const float right_vertex    =  (width  / 2);
    const float top_vertex      =  (height / 2);
    const float bottom_vertex   = -(height / 2);
    
    const float left_uv_coord   = 0.0f;
    const float right_uv_coord  = 1.0f;
    const float bottom_uv_coord = 1.0f;
    const float top_uv_coord    = 0.0f;
    
    recipient->x = projected_mid_x;
    recipient->y = projected_mid_y;
    recipient->z = z;
    recipient->visible = true;
    
    // **
    // top & left triangle
    // **
    // top left vertex
    recipient->triangles[0].vertices[0].x     = left_vertex;
    recipient->triangles[0].vertices[0].y     = top_vertex;
    recipient->triangles[0].vertices[0].z     = 0.0f;
    recipient->triangles[0].vertices[0].uv[0] = left_uv_coord;
    recipient->triangles[0].vertices[0].uv[1] = top_uv_coord;
    // top right vertex
    recipient->triangles[0].vertices[1].x     = right_vertex;
    recipient->triangles[0].vertices[1].y     = top_vertex;
    recipient->triangles[0].vertices[1].z     = 0.0f;
    recipient->triangles[0].vertices[1].uv[0] = right_uv_coord;
    recipient->triangles[0].vertices[1].uv[1] = top_uv_coord;
    // bottom left vertex
    recipient->triangles[0].vertices[2].x     = left_vertex;
    recipient->triangles[0].vertices[2].y     = bottom_vertex;
    recipient->triangles[0].vertices[2].z     = 0.0f;
    recipient->triangles[0].vertices[2].uv[0] = left_uv_coord;
    recipient->triangles[0].vertices[2].uv[1] = bottom_uv_coord;
    
    recipient->triangles[0].normal.x       = 0.0f;
    recipient->triangles[0].normal.y       = 0.0f;
    recipient->triangles[0].normal.z       = 1.0f;
    recipient->triangles[0].texturearray_i = -1;
    recipient->triangles[0].texture_i      = -1;
    recipient->triangles[0].color[0]       = 1.0f;
    recipient->triangles[0].color[1]       = 1.0f;
    recipient->triangles[0].color[2]       = 1.0f;
    recipient->triangles[0].color[3]       = 1.0f;
    
    // **
    // right & bottom triangle
    // **
    // top right vertex
    recipient->triangles[1].vertices[0].x     = right_vertex;
    recipient->triangles[1].vertices[0].y     = top_vertex;
    recipient->triangles[1].vertices[0].z     = 0.0f;
    recipient->triangles[1].vertices[0].uv[0] = right_uv_coord;
    recipient->triangles[1].vertices[0].uv[1] = top_uv_coord;
    // bottom right vertex
    recipient->triangles[1].vertices[1].x     = right_vertex;
    recipient->triangles[1].vertices[1].y     = bottom_vertex;
    recipient->triangles[1].vertices[1].z     = 0.0f;
    recipient->triangles[1].vertices[1].uv[0] = right_uv_coord;
    recipient->triangles[1].vertices[1].uv[1] = bottom_uv_coord;
    // bottom left vertex
    recipient->triangles[1].vertices[2].x     = left_vertex;
    recipient->triangles[1].vertices[2].y     = bottom_vertex;
    recipient->triangles[1].vertices[2].z     = 0.0f;
    recipient->triangles[1].vertices[2].uv[0] = left_uv_coord;
    recipient->triangles[1].vertices[2].uv[1] = bottom_uv_coord;
    
    recipient->triangles[1].normal.x       = 0.0f;
    recipient->triangles[1].normal.y       = 0.0f;
    recipient->triangles[1].normal.z       = 1.0f;
    recipient->triangles[1].texturearray_i = -1;
    recipient->triangles[1].texture_i      = -1;
    recipient->triangles[1].color[0]       = 1.0f;
    recipient->triangles[1].color[1]       = 1.0f;
    recipient->triangles[1].color[2]       = 1.0f;
    recipient->triangles[1].color[3]       = 1.0f;
}
