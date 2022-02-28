#import "gpu.h"

@implementation MetalKitViewDelegate
{
    NSUInteger _currentFrameIndex;
}

- (void)configureMetal
{
    _currentFrameIndex = 0;
   
}

- (void)drawInMTKView:(MTKView *)view
{
    // TODO: this only works on retina
    // because on retina screens, the MTLViewport is 2x
    // the size of the window
    MTLViewport viewport = {
        0,
        0,
        window_width * 2.0f,
        window_height * 2.0f };
    
    uint32_t frame_i = _currentFrameIndex;
    
    Vertex * vertices_for_gpu =
        _render_commands.vertex_buffers[frame_i].vertices;
    uint32_t vertices_for_gpu_size = 0;
    
    software_render(
        /* next_gpu_workload: */
            vertices_for_gpu,
        /* next_gpu_workload_size: */
            &vertices_for_gpu_size);
    
    @autoreleasepool 
    {
        id<MTLCommandBuffer> command_buffer =
            [[self command_queue] commandBuffer];
        
        MTLRenderPassDescriptor *RenderPassDescriptor =
            [view currentRenderPassDescriptor];
        RenderPassDescriptor.colorAttachments[0].loadAction =
            MTLLoadActionClear;
        
        MTLClearColor clear_color =
            MTLClearColorMake(0.2f, 0.2f, 0.1f, 1.0f);
        RenderPassDescriptor.colorAttachments[0].clearColor =
            clear_color;
        
        id<MTLRenderCommandEncoder> render_encoder =
            [command_buffer
                renderCommandEncoderWithDescriptor:
                    RenderPassDescriptor];
        [render_encoder setViewport: viewport];
       
        // encode the drawing of all triangles 
        id<MTLBuffer> current_buffered_vertices =
            [[self vertex_buffers]
                objectAtIndex: _currentFrameIndex];
        [render_encoder
            setVertexBuffer: current_buffered_vertices  
            offset: 0 
            atIndex: 0];
        [render_encoder
            setRenderPipelineState:
                [self combo_pipeline_state]];
        [render_encoder
            setFragmentTexture:_metal_textures[0]
            atIndex:0];
        [render_encoder
            drawPrimitives: MTLPrimitiveTypeTriangle
            vertexStart: 0 
            vertexCount: vertices_for_gpu_size];
        
        [render_encoder endEncoding];
        
        // Schedule a present once the framebuffer is complete
        // using the current drawable
        id<CAMetalDrawable> current_drawable =
            [view currentDrawable];
        [command_buffer presentDrawable: current_drawable];
        
        uint32_t next_index = _currentFrameIndex + 1;
        if (next_index > 2) { next_index = 0; }
        
        _currentFrameIndex = next_index;
        
        /* 
        [command_buffer
            addCompletedHandler:
                ^(id<MTLCommandBuffer> commandBuffer)
            {
            }];
        */
        
        [command_buffer commit];
    }
}

- (void)mtkView:(MTKView *)view
    drawableSizeWillChange:(CGSize)size
{

}

@end
