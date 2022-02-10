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
    printf("drawInMTKView...\n");
    // TODO: this only works on retina
    // because on retina screens, the MTLViewport is 2x
    // the size of the window
    MTLViewport viewport = {
        0,
        0,
        WINDOW_WIDTH,
        WINDOW_HEIGHT };
    
    uint32_t frame_i = _currentFrameIndex;
    
    ColoredVertex * vertices_for_gpu =
        _render_commands.vertex_buffers[frame_i].vertices;
    uint32_t vertices_for_gpu_size = 0;
    
    software_render(
        /* next_gpu_workload: */
            vertices_for_gpu,
        /* next_gpu_workload_size: */
            &vertices_for_gpu_size);
    
    @autoreleasepool 
    {
        id<MTLCommandBuffer> CommandBuffer =
            [[self command_queue] commandBuffer];
        
        MTLRenderPassDescriptor *RenderPassDescriptor =
            [view currentRenderPassDescriptor];
        RenderPassDescriptor.colorAttachments[0].loadAction =
            MTLLoadActionClear;
        
        MTLClearColor MetalClearColor =
            MTLClearColorMake(0.3f, 0.3f, 0.0f, 1.0f);
        RenderPassDescriptor.colorAttachments[0].clearColor =
            MetalClearColor;
        
        id<MTLRenderCommandEncoder> RenderEncoder =
            [CommandBuffer
                renderCommandEncoderWithDescriptor:
                    RenderPassDescriptor];
        [RenderEncoder setViewport: viewport];
        [RenderEncoder
            setRenderPipelineState:
                [self solid_color_pipeline_state]];
        
        id<MTLBuffer> MacBufferedVertex =
            [[self mac_vertex_buffers]
                objectAtIndex: _currentFrameIndex];
        
        [RenderEncoder
            setVertexBuffer: MacBufferedVertex  
            offset: 0 
            atIndex: 0];
        
        [RenderEncoder
            drawPrimitives: MTLPrimitiveTypeTriangle
            vertexStart: 0 
            vertexCount: vertices_for_gpu_size];
        
        [RenderEncoder endEncoding];
        
        // Schedule a present once the framebuffer is complete
        // using the current drawable
        id<CAMetalDrawable> NextDrawable =
            [view currentDrawable];
        [CommandBuffer presentDrawable: NextDrawable];
        
        uint32_t NextIndex = _currentFrameIndex + 1;
        
        if (NextIndex > 2)
        {
            NextIndex = 0;
        }
        
        _currentFrameIndex = NextIndex;
        
        /* 
        [CommandBuffer
            addCompletedHandler:
                ^(id<MTLCommandBuffer> commandBuffer)
            {
            }];
        */
        
        [CommandBuffer commit];
    }
}

- (void)mtkView:(MTKView *)view
    drawableSizeWillChange:(CGSize)size
{

}

@end
