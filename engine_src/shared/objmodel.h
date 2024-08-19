#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <math.h>

#include "clientlogic_macro_settings.h"
#include "cpu_gpu_shared_types.h"

#include "common.h"
#include "logger.h"
#include "triangle.h"
#include "objparser.h"
#include "platform_layer.h"
#include "memorystore.h"


#ifdef __cplusplus
extern "C" {
#endif

#define BASIC_QUAD_MESH_ID 0
#define BASIC_CUBE_MESH_ID 1
#define BASIC_POINT_MESH_ID 2 // There's currently no way to draw points
#define BASIC_LINE_MESH_ID 3 // Drawing lines is became very expensive

#define OBJ_STRING_SIZE 128
typedef struct MeshSummary {
    char resource_name[OBJ_STRING_SIZE]; // the resource filename (without path)
    int32_t mesh_id;
    int32_t vertices_head_i;
    int32_t vertices_size;
    float base_width;
    float base_height;
    float base_depth;
    int32_t shattered_vertices_head_i; // -1 if no shattered version
    int32_t shattered_vertices_size; // 0 if no shattered version
    char material_names[MAX_MATERIALS_PER_POLYGON][OBJ_STRING_SIZE];
    uint32_t materials_size;
} MeshSummary;

typedef struct LockedVertexWithMaterialCollection {
    GPULockedVertex gpu_data[ALL_LOCKED_VERTICES_SIZE];
    uint32_t size;
} LockedVertexWithMaterialCollection;

extern MeshSummary * all_mesh_summaries;
extern uint32_t all_mesh_summaries_size;

extern LockedVertexWithMaterialCollection * all_mesh_vertices;

void init_all_meshes(void);

int32_t new_mesh_id_from_obj_text(
    const char * obj_text,
    const uint32_t expected_materials_count,
    const char expected_materials_names[MAX_MATERIALS_PER_POLYGON][256]);

int32_t new_mesh_id_from_resource_asserts(
    const char * filename,
    const uint32_t expected_materials_count,
    const char expected_materials_names[MAX_MATERIALS_PER_POLYGON][256]);

int32_t new_mesh_id_from_resource(
    const char * filename);

void center_mesh_offsets(const int32_t mesh_id);

void flip_mesh_uvs(const int32_t mesh_id);

/*
Creates a version of the mesh with (normally needless) extra triangles

This can be useful if you have a very large flat area that needs to catch
lights in between its vertices, or if you plan to 'shatter' or explode the
mesh into many pieces.

After running this function, the new triangles can be found in:
all_mesh_summaries[your_mesh_id].shattered_triangles_head_i;
all_mesh_summaries[your_mesh_id].shattered_triangles_size;
*/
void create_shattered_version_of_mesh(
    const int32_t mesh_id,
    const uint32_t triangles_mulfiplier);

#ifdef __cplusplus
}
#endif

#endif // OBJMODEL_H
