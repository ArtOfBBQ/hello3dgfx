#include <metal_stdlib>
#include <simd/simd.h>
#include "../shared/window_size.h"

using namespace metal;

typedef struct
{
    float4 position [[position]];
    float4 color;

} RasterizerData;

struct metal_vertex
{
    float4 position;
    float4 color;
};

vertex RasterizerData
vertexShader(
    uint vertexID [[ vertex_id ]],
    constant metal_vertex *vertexArray [[ buffer(0) ]])
{
    RasterizerData out;
    float2 pixelSpacePosition =
        vertexArray[vertexID].position.xy;
    
    // IMPORTANT: (Ted) Viewport size was not passed into the
    // vertex shader.
    float2 normalizedPosition =
        (pixelSpacePosition / (WINDOW_WIDTH / 2.0)) - 1;
    
    // To convert from positions in pixel space to positions in
    // clip-space, divide the pixel coordinates by half the size
    // of the viewport.
    // Z is set to 0.0 and w to 1.0 because this is 2D sample.
    out.position =
        vector_float4(
            normalizedPosition.x,
            normalizedPosition.y,
            0.0,
            1.0);
    
    out.color = vertexArray[vertexID].color;
    
    return out;
}

// Fragment function
fragment float4 fragmentShader(RasterizerData in [[stage_in]])
{
    // Return the interpolated color.
    return in.color;
}

