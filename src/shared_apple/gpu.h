#ifndef GPU_H
#define GPU_H

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <sys/mman.h>
#import <mach/mach_time.h>

#include "../shared/texture_array.h"
#include "../shared/vertex_types.h"
#include "../shared/vertex_buffer_types.h"
#include "../shared/window_size.h"
#include "../shared/scheduled_animations.h"
#include "../shared/software_renderer.h"
#include "../shared/bitmap_renderer.h"


@interface MetalKitViewDelegate: NSObject<MTKViewDelegate>
@property (retain) id<MTLDevice> metal_device;
@property (retain) id<MTLCommandQueue> command_queue;
@property (retain) NSMutableArray * vertex_buffers;
@property VertexBuffer render_commands;
@property (retain) id<MTLRenderPipelineState>
    combo_pipeline_state;
@property (retain) NSMutableArray * metal_textures;
- (void)
    updateTextureArray: (int32_t)texturearray_i;
- (void)
    configureMetalWithDevice: (id<MTLDevice>)metal_device
    andPixelFormat: (MTLPixelFormat)pixel_format
    fromFolder: (NSString *)shader_lib_filepath;
@end

extern MetalKitViewDelegate * apple_gpu_delegate;
extern uint64_t previous_time;

#endif

