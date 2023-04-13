#ifndef CLIENTLOGIC_MACRO_SETTINGS
#define CLIENTLOGIC_MACRO_SETTINGS

/*
This header should only contain macro definitions that modify the behavior
of the engine for your specific app.
*/

// 290mb ->                   290...000
#define UNMANAGED_MEMORY_SIZE 290000000
// 90 mb ->                    90...000
#define MANAGED_MEMORY_SIZE    90000000

#define APPLICATION_NAME "TOK ONE"

#define INITIAL_WINDOW_HEIGHT   800
#define INITIAL_WINDOW_WIDTH   1200
#define INITIAL_WINDOW_LEFT     300
#define INITIAL_WINDOW_BOTTOM   100

/*
The maximum number of sprites/meshes in your app.
*/
#define ZPOLYGONS_TO_RENDER_ARRAYSIZE 500

/*
the max # of triangles in 1 zpolygon. 2 is enough for an app that literally
only has 2D sprites (quads)
*/
#define ALL_MESH_TRIANGLES_SIZE 10000

/*
The max # of simultaneously active particle effects in your app
*/
#define PARTICLE_EFFECTS_SIZE 10


#endif // CLIENTLOGIC_MACRO_SETTINGS
