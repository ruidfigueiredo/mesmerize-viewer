#pragma once
#include <GL/glew.h>
#include <iostream>
#include <signal.h>

#define ASSERT(x) \
    if (!(x))     \
    raise(SIGTRAP)

#define GL_CALL(x)   \
    ClearGlErrors(); \
    x;               \
    ASSERT(GlLogCall(#x, __FILE__, __LINE__))

static void
ClearGlErrors()
{
    while (glGetError())
        ;
}

static void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

static bool GlLogCall(const char *functionCall, const char *file, int lineNo)
{
    GLenum error = glGetError();
    if (error == GL_NO_ERROR)
        return true;

    std::cout << "[OpenGL Error] ("
              << std::hex
              << error
              << std::dec << ") "
              << functionCall << " "
              << file << ": " << lineNo << std::endl;
    return false;
}
