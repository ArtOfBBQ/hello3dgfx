#include <metal_stdlib>
#include <simd/simd.h>

#include "../shared/cpu_gpu_shared_types.h"

using namespace metal;

typedef struct
{
    float4 position [[position]];
    float4 color;
    float2 texture_coordinate;
    float3 lighting;
    int texturearray_i;
    int texture_i;
    float point_size [[point_size]];
} RasterizerPixel;

float3 x_rotate(float3 vertices, float x_angle) {
    float3 rotated_vertices = vertices;
    float cos_angle = cos(x_angle);
    float sin_angle = sin(x_angle);
    
    rotated_vertices[1] =
        vertices[1] * cos_angle -
        vertices[2] * sin_angle;
    rotated_vertices[2] =
        vertices[1] * sin_angle +
        vertices[2] * cos_angle;
    
    return rotated_vertices;
}

float3 y_rotate(float3 vertices, float y_angle) {
    float3 rotated_vertices = vertices;
    float cos_angle = cos(y_angle);
    float sin_angle = sin(y_angle);
    
    rotated_vertices[0] =
        vertices[0] * cos_angle +
        vertices[2] * sin_angle;
    rotated_vertices[2] =
        vertices[2] * cos_angle -
        vertices[0] * sin_angle;
    
    return rotated_vertices;
}

float3 z_rotate(float3 vertices, float z_angle) {
    float3 rotated_vertices = vertices;
    float cos_angle = cos(z_angle);
    float sin_angle = sin(z_angle);
    
    rotated_vertices[0] =
        vertices[0] * cos_angle -
        vertices[1] * sin_angle;
    rotated_vertices[1] =
        vertices[1] * cos_angle +
        vertices[0] * sin_angle;
    
    return rotated_vertices;
}

float get_distance(
    float3 a,
    float3 b)
{
    float3 squared_diffs = (a-b)*(a-b);
    
    float sum_squares = dot(
        squared_diffs,
        vector_float3(1.0f,1.0f,1.0f));
    
    return sqrt(sum_squares);
}

vertex RasterizerPixel
vertex_shader(
    uint vertex_i [[ vertex_id ]],
    const device GPUVertex * vertices [[ buffer(0) ]],
    const device GPUPolygonCollection * polygon_collection [[ buffer(1) ]],
    const device GPULightCollection * light_collection [[ buffer(2) ]],
    const device GPUCamera * camera [[ buffer(3) ]],
    const device GPULockedVertex * locked_vertices [[ buffer(4) ]],
    const device GPUProjectionConstants * projection_constants [[ buffer(5) ]],
    const device GPUPolygonMaterial * polygon_materials [[ buffer(6) ]])
{
    RasterizerPixel out;
    
    uint polygon_i = vertices[vertex_i].polygon_i;
    uint locked_vertex_i = vertices[vertex_i].locked_vertex_i;
    uint locked_material_i = (polygon_i * MAX_MATERIALS_PER_POLYGON) +
        locked_vertices[locked_vertex_i].parent_material_i;
    
    float3 parent_mesh_position = vector_float3(
        polygon_collection->polygons[polygon_i].xyz[0],
        polygon_collection->polygons[polygon_i].xyz[1],
        polygon_collection->polygons[polygon_i].xyz[2]);
    
    float3 mesh_vertices = vector_float3(
        locked_vertices[locked_vertex_i].xyz[0],
        locked_vertices[locked_vertex_i].xyz[1],
        locked_vertices[locked_vertex_i].xyz[2]);
    
    float3 vertex_multipliers = vector_float3(
        polygon_collection->polygons[polygon_i].xyz_multiplier[0],
        polygon_collection->polygons[polygon_i].xyz_multiplier[1],
        polygon_collection->polygons[polygon_i].xyz_multiplier[2]);
    
    float3 vertex_offsets = vector_float3(
        polygon_collection->polygons[polygon_i].xyz_offset[0],
        polygon_collection->polygons[polygon_i].xyz_offset[1],
        polygon_collection->polygons[polygon_i].xyz_offset[2]);
    
    mesh_vertices *= vertex_multipliers;
    mesh_vertices += vertex_offsets;
    
    mesh_vertices *= polygon_collection->polygons[polygon_i].scale_factor;
    
    float3 mesh_normals = vector_float3(
        locked_vertices[locked_vertex_i].normal_xyz[0],
        locked_vertices[locked_vertex_i].normal_xyz[1],
        locked_vertices[locked_vertex_i].normal_xyz[2]);
    
    // rotate vertices
    float3 x_rotated_vertices = x_rotate(
        mesh_vertices,
        polygon_collection->polygons[polygon_i].xyz_angle[0]);
    float3 x_rotated_normals  = x_rotate(
        mesh_normals,
        polygon_collection->polygons[polygon_i].xyz_angle[0]);
    
    float3 y_rotated_vertices = y_rotate(
        x_rotated_vertices,
        polygon_collection->polygons[polygon_i].xyz_angle[1]);
    float3 y_rotated_normals  = y_rotate(
        x_rotated_normals,
        polygon_collection->polygons[polygon_i].xyz_angle[1]);
    
    float3 z_rotated_vertices = z_rotate(
        y_rotated_vertices,
        polygon_collection->polygons[polygon_i].xyz_angle[2]);
    float3 z_rotated_normals  = z_rotate(
        y_rotated_normals,
        polygon_collection->polygons[polygon_i].xyz_angle[2]);
    
    // translate to world position
    float3 rotated_pos = z_rotated_vertices + parent_mesh_position;
    
    // polygon_collection->polygons[polygon_i].ignore_camera
    float3 camera_position = vector_float3(
        camera->xyz[0],
        camera->xyz[1],
        camera->xyz[2]);
    float3 camera_translated_pos = rotated_pos - camera_position;
    
    // rotate around camera
    float3 cam_x_rotated = x_rotate(
        camera_translated_pos,
        -camera->xyz_angle[0]);
    float3 cam_y_rotated = y_rotate(
        cam_x_rotated,
        -camera->xyz_angle[1]);
    float3 cam_z_rotated = z_rotate(
        cam_y_rotated,
        -camera->xyz_angle[2]);
    
    float ignore_cam =
        polygon_collection->polygons[polygon_i].ignore_camera;
    out.position = vector_float4(
        (cam_z_rotated * (1.0f - ignore_cam)) +
        (rotated_pos * ignore_cam),
        1.0f);
    
    // projection
    out.position[0] *= projection_constants->x_multiplier;
    out.position[1] *= projection_constants->field_of_view_modifier;
    out.position[3]  = out.position[2];
    out.position[2]  =     
        (out.position[2] * projection_constants->q) -
        (projection_constants->znear * projection_constants->q);
    
    out.color = vector_float4(
        polygon_materials[locked_material_i].rgba[0],
        polygon_materials[locked_material_i].rgba[1],
        polygon_materials[locked_material_i].rgba[2],
        polygon_materials[locked_material_i].rgba[3]);
    
    float4 bonus_rgb = vector_float4(
        polygon_collection->polygons[polygon_i].bonus_rgb[0],
        polygon_collection->polygons[polygon_i].bonus_rgb[1],
        polygon_collection->polygons[polygon_i].bonus_rgb[2],
        0.0f);
    
    out.color += bonus_rgb;
    
    /* out.texturearray_i = vertices[vertex_i].texturearray_i; */
    out.texturearray_i = polygon_materials[locked_material_i].texturearray_i;
    /* out.texture_i = vertices[vertex_i].texture_i; */
    out.texture_i = polygon_materials[locked_material_i].texture_i;
    
    out.texture_coordinate = vector_float2(
        locked_vertices[locked_vertex_i].uv[0],
        locked_vertices[locked_vertex_i].uv[1]);
    
    out.lighting = float3(0.0f, 0.0f, 0.0f);
    for (
        uint32_t i = 0;
        i < light_collection->lights_size;
        i++)
    {
        // ambient lighting
        float3 light_pos = vector_float3(
            light_collection->light_x[i],
            light_collection->light_y[i],
            light_collection->light_z[i]);
        float3 light_color = vector_float3(
            light_collection->red[i],
            light_collection->green[i],
            light_collection->blue[i]);
        float distance = get_distance(
            light_pos,
            rotated_pos);
        float distance_mod = (light_collection->reach[i] + 0.05f)
            - (distance * distance);
        distance_mod = clamp(distance_mod, 0.0f, 1.5f);
        
        out.lighting += (
            distance_mod *
            light_color *
            light_collection->ambient[i]);
        
        // diffuse lighting
        z_rotated_normals = normalize(z_rotated_normals);
        
        // if light is at 2,2,2 and rotated_pos is at 1,1,1, we need +1/+1/+1
        // to go from the rotated_pos to the light
        // if the normal also points to the light, we want more diffuse
        // brightness
        float3 object_to_light = normalize(
            (light_pos - rotated_pos));
        
        float diffuse_dot =
            max(
                dot(
                    z_rotated_normals,
                    object_to_light),
                0.0f);
        
        out.lighting += (
            light_color *
            distance_mod *
            (light_collection->diffuse[i] * 1.25f) *
            diffuse_dot);
        
        // specular lighting
        float3 object_to_view = normalize(
            camera_position - rotated_pos);
        
        float3 reflection_ray = reflect(
            -object_to_light,
            z_rotated_normals);
        
        float spec = pow(max(dot(object_to_view, reflection_ray), 0.0), 32);
        out.lighting += (
            1.5f * spec * light_color * distance_mod);
    }
    
    out.lighting = clamp(out.lighting, 0.05f, 7.5f);
    
    float ignore_light =
        polygon_collection->polygons[polygon_i].ignore_lighting;
    
    float3 all_ones = vector_float3(1.0f, 1.0f, 1.0f);
    out.lighting =
        ((1.0f - ignore_light) * out.lighting) +
        (ignore_light * all_ones);
    
    out.point_size = 40.0f;
    
    return out;
}

fragment float4
fragment_shader(
    RasterizerPixel in [[stage_in]],
    array<texture2d_array<half>, TEXTUREARRAYS_SIZE>
        color_textures[[ texture(0) ]])
{
    float4 out_color = in.color;
    
    if (
        in.texturearray_i < 0 ||
        in.texture_i < 0)
    {
        out_color *= vector_float4(in.lighting, 1.0f);
    } else {
        constexpr sampler textureSampler(
            mag_filter::nearest,
            min_filter::nearest);
        
        // Sample the texture to obtain a color
        const half4 color_sample =
        color_textures[in.texturearray_i].sample(
            textureSampler,
            in.texture_coordinate, 
            in.texture_i);
        float4 texture_sample = float4(color_sample);
        
        out_color *= texture_sample * vector_float4(in.lighting, 1.0f);;
    }
    
    int diamond_size = 35.0f;
    int neghalfdiamond = -1.0f * (diamond_size / 2);
    
    int alpha_tresh = (int)(out_color[3] * diamond_size);
    
    if (
        out_color[3] < 0.05f ||
        (
            out_color[3] < 0.95f &&
            (
                abs((neghalfdiamond + (int)in.position.x % diamond_size)) +
                abs((neghalfdiamond + (int)in.position.y % diamond_size))
            ) > alpha_tresh
        ))
    {
        discard_fragment();
        return out_color;
    }
    
    out_color[3] = 1.0f;
    return out_color;
}

fragment float4
alphablending_fragment_shader(
    RasterizerPixel in [[stage_in]],
    array<texture2d_array<half>, TEXTUREARRAYS_SIZE>
        color_textures[[ texture(0) ]])
{
    float4 out_color = in.color;
    
    if (
        in.texturearray_i < 0 ||
        in.texture_i < 0)
    {
        out_color *= vector_float4(in.lighting, 1.0f);
    } else {
        constexpr sampler textureSampler(
            mag_filter::nearest,
            min_filter::nearest);
        
        // Sample the texture to obtain a color
        const half4 color_sample =
        color_textures[in.texturearray_i].sample(
            textureSampler,
            in.texture_coordinate, 
            in.texture_i);
        float4 texture_sample = float4(color_sample);
        
        out_color *= texture_sample * vector_float4(in.lighting, 1.0f);;
    }
    
    return out_color;
}
