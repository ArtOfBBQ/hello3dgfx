#include "opengl.h"

// We'll need these 2 identifiers while drawing
GLuint program_id;
unsigned int VAO = UINT32_MAX;
unsigned int VBO = UINT32_MAX;
void opengl_render_triangles(void) {
    
    assert(VAO < UINT32_MAX);
    assert(VBO < UINT32_MAX);
    
    
    // glPointSize(50); // for GL_POINTS
    glDrawArrays(
        /* GLenum mode: */
            GL_TRIANGLES,
        /* GLint first: */
            0,
        /* GLint count (# of vertices to render): */
            3);
}

static void opengl_compile_given_shader(
    GLuint shader_id,
    char * shader_source,
    GLint source_length)
{
    glShaderSource(
        /* GLuint handle: */
            shader_id,
        /* shader count : */
            1,
        /* const GLchar ** string: */
            &shader_source,
        /* const GLint * length: */
            &source_length);
    
    glCompileShader(shader_id);
    
    GLint is_compiled = INT8_MAX;
    char info_log[512];
    info_log[0] = '\0';
    glGetShaderiv(
        /* GLuint id: */
            shader_id,
        /* GLenum pname: */
            GL_COMPILE_STATUS,
        /* GLint * params: */
            &is_compiled);
    
    if (is_compiled == GL_FALSE) {
        printf("%s\n", "failed to compile shader with source: ");
        printf("%s\n", "*******");
        printf("%s\n", shader_source);
        printf("%s\n", "*******");
        GLenum err_value = glGetError();
        printf("glGetError returned: %u\n", err_value);
        printf("%s\n", "*******");
        
        glGetShaderInfoLog(
            /* GLuint shader id: */
                shader_id,
            /* GLsizei max length: */
                512,
            /* GLsizei * length: */
                NULL,
            /* GLchar * infolog: */
                info_log);
        printf("shader info log: %s\n", info_log);
        printf("%s\n", "*******");
        assert(0);
    } else if (is_compiled == GL_TRUE) {
        
    } else {
        printf(
            "glGetShaderiv() returned an impossible value. Expected GL_TRUE "
            "(%i) or GL_FALSE (%i), got: %i\n",
            GL_TRUE,
            GL_FALSE,
            is_compiled);
        assert(0);
    }
}

void opengl_compile_shaders(
    char * vertex_shader_source,
    uint32_t vertex_shader_source_size,
    char * fragment_shader_source,
    uint32_t fragment_shader_source_size)
{
    // opengl_compile_shaders()...
    
    // allocate buffer memory...
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLenum err_value;
    
    assert(vertex_shader_source_size > 0);
    assert(vertex_shader_source != NULL);
    
    opengl_compile_given_shader(
        /* GLuint shader_id: */
            vertex_shader_id,
        /* char * shader_source: */
            vertex_shader_source,
        /* GLint source_length: */
            vertex_shader_source_size);
    
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    assert(fragment_shader_source_size > 0);
    assert(fragment_shader_source != NULL);
    opengl_compile_given_shader(
        /* GLuint shader_id: */
            fragment_shader_id,
        /* char * shader_source: */
            fragment_shader_source,
        /* GLint source_length: */
            fragment_shader_source_size);
    
    // attach compiled shaders to program
    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    
    GLint success = -1;
    glGetProgramiv(
        /* GLuint program id: */
            program_id,
        /* GLenum pname: */
            GL_LINK_STATUS,
        /* GLint * params: */
            &success);
    assert(success);
    
    err_value = glGetError();
    assert(err_value == GL_NO_ERROR);
    
    glGenVertexArrays(1, &VAO);
    err_value = glGetError();
    assert(err_value == GL_NO_ERROR);
    
    glGenBuffers(1, &VBO);
    err_value = glGetError();
    assert(err_value == GL_NO_ERROR);

    glBindVertexArray(VAO);
    err_value = glGetError();
    assert(err_value == GL_NO_ERROR);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    err_value = glGetError();
    assert(err_value == GL_NO_ERROR);

    // TODO: Learn exactly when nescessary, I hope we can just set & forget
    glUseProgram(program_id);
    err_value = glGetError();
    assert(err_value == GL_NO_ERROR);
    
    glBindVertexArray(VAO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    err_value = glGetError();
    assert(err_value == GL_NO_ERROR);
    
    // glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);
    
    GPUVertex data[3];
    data[0].x = 0.25f;
    data[0].y = 0.75f;
    
    data[1].x = 0.75f;
    data[1].y = 0.75f;
    
    data[2].x = 0.50f;
    data[2].y = 0.25f;
    
    err_value = glGetError();
    assert(err_value == GL_NO_ERROR);
    
    glBufferData(
        /* target: */
            GL_ARRAY_BUFFER,
        /* size_in_bytes: */
            (sizeof(GPUVertex) * 3),
        /* const GLvoid * data: (to init with, or NULL to copy no data) */
            (const GLvoid *)data,
        /* usage: */
            GL_STATIC_DRAW);
    
    err_value = glGetError();
    if (err_value != GL_NO_ERROR) {
        switch (err_value) {
            case GL_INVALID_VALUE:
                printf("%s\n", "GL_INVALID_VALUE");
                break;
            case GL_INVALID_ENUM:
                printf("%s\n", "GL_INVALID_ENUM");
                break;
            case GL_INVALID_OPERATION:
                printf("%s\n", "GL_INVALID_OPERATION");
                break;
            default:
                printf("%s\n", "unhandled!");
                break;
        }
        assert(0);
    }
    
    
    /*
    Attribute pointers describe the fields of our data
    sructure (the Vertex struct in shared/vertex_types.h)
    */
    // struct field: float x;
    assert(sizeof(GPUVertex) == 96);
    assert(offsetof(GPUVertex, x) == 0); 
    glVertexAttribPointer(
        /* GLuint index (location in shader source): */
            0,
        /* GLint size (number of components per vertex, must be 1-4): */
            1,
        /* GLenum type (of data): */
            GL_FLOAT,
        /* GLboolean normalize data: */
            GL_FALSE,
        /* GLsizei stride (to next 'x'): */
            sizeof(GPUVertex),
        /* const GLvoid * pointer (offset) : */
            (void *)(0));
    
    err_value = glGetError();
    if (err_value != GL_NO_ERROR) {
        switch (err_value) {
            case GL_INVALID_VALUE:
                printf("%s\n", "GL_INVALID_VALUE");
                break;
            case GL_INVALID_ENUM:
                printf("%s\n", "GL_INVALID_ENUM");
                break;
            case GL_INVALID_OPERATION:
                printf("%s\n", "GL_INVALID_OPERATION");
                break;
            default:
                printf("%s\n", "unhandled!");
        }
        assert(0);
    }
    
    assert(sizeof(GLfloat) == 4);
    assert(sizeof(float) == 4);
    assert(offsetof(GPUVertex, y) == 4); 
    // struct field: float x;
    glVertexAttribPointer(
        /* GLuint index (in shader source): */
            1,
        /* GLint size (number of components per vertex, must be 1-4): */
            1,
        /* GLenum type (of data): */
            GL_FLOAT,
        /* GLboolean normalize data: */
            GL_FALSE,
        /* GLsizei stride: */
            sizeof(GPUVertex),
        /* const GLvoid * pointer (offset) : */
            (void*)(4));
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
}

