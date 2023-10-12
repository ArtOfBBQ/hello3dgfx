#ifndef CPU_TO_GPU_TYPES_H
#define CPU_TO_GPU_TYPES_H

#include "cpu_gpu_shared_types.h"
#include "common.h"
#include "clientlogic_macro_settings.h"

// This is a bunch of pointers because apple's metal (both ios and macosx)
// requires shared data to be aligned to page size :(
typedef struct GPUDataForSingleFrame
{
    GPUVertex *                            vertices;
    uint32_t                          vertices_size;
    GPULightCollection *           light_collection;
    GPUCamera *                              camera;
    GPUProjectionConstants *   projection_constants;
} GPUDataForSingleFrame;

typedef struct GPUSharedDataCollection
{
    uint32_t frame_i;
    GPUDataForSingleFrame triple_buffers[3];
} GPUSharedDataCollection;

extern GPUSharedDataCollection gpu_shared_data_collection;

#endif // CPU_TO_GPU_TYPES_H

