#ifndef BITMAP_RENDERER_H
#define BITMAP_RENDERER_H

/*
Directly adjust pixels in a bitmap image, then copy that image
to gpu memory and display it to the screen as is.

For now I just want this to draw a little 2D minimap in
the corner of the screen to help me understand what's going on
with my 3D objects

You could delete all of the 3d graphics code and use this to
draw a bitmap over the entire screen and update it every frame,
so you can enjoy retro game programming from when gpu's didn't
exist.
*/

#include "clientlogic.h"
#include "zpolygon.h"
#include "platform_layer.h"
#include "decodedimage.h"
#include "vertex_types.h"
#include "draw_triangle.h"


/* Draw bitmap(s) of pixels and add them to the gpu's workload */
void render_bitmaps(
    Vertex * next_gpu_workload,
    uint32_t * next_gpu_workload_size);

#endif

