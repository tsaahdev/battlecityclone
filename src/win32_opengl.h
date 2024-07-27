#pragma once

#include "common.h"

#include "win32_opengl_defines.h"


namespace base::opengl {



namespace internal {

::HMODULE library{ nullptr };
::HWND helperWindowHandle{ nullptr };
::HDC wglDC{ nullptr };
::HGLRC wglHandle{ nullptr };

struct State {
    b8 ARB_pixelFormat{ false };
    b8 ARB_multisample{ false };
    b8 ARB_framebuffer_srgb{ false };
    b8 EXT_framebuffer_srgb{ false };
    b8 ARB_create_context{ false };
    b8 ARB_create_context_profile{ false };
    b8 ARB_create_context_robustness{ false };
    b8 ARB_context_flush_cntrol{ false };
    b8 ARB_context_no_error{ false };
} state;

inline b8 createHelperWindow(::HINSTANCE instance);
inline b8 initWgl();
inline b8 createContext(::HWND windowHandle);
inline b8 loadOpenGlFunctions();
inline void messageCallback(u32 source, u32 type, u32 id, u32 severity, i32 length, const c8* message, void* userParam);

u32 wip_vao{ 0 };
u32 wip_vbo{ 0 };
u32 wip_program{ 0 };

} // namespace internal

inline b8 init(::HINSTANCE instance, ::HWND windowHandle) {
    using namespace internal;
    // actual wgl -> modern opengl context creation was peeked from GLFW.
    // https://www.glfw.org/

    if (!createHelperWindow(instance)) { return false; }
    if (!initWgl()) { return false; }
    if (!createContext(windowHandle)) { return false; }
    if (!wglMakeCurrent(wglDC, wglHandle)) { return false; }
    if (!loadOpenGlFunctions()) { return false; }

    #if defined(E_DEBUG_BUILD)
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(messageCallback, nullptr);
    #endif

    #if defined(E_DEBUG_BUILD)
        i32 majorVersion{ 0 };
        i32 minorVersion{ 0 };
        glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
        glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
        E_PRINT("OpenGL vendor: %s", glGetString(GL_VENDOR));
        E_PRINT("OpenGL renderer: %s", glGetString(GL_RENDERER));
        E_PRINT("OpenGL version: %s", glGetString(GL_VERSION));
        E_PRINT("OpenGL shading language version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
        E_PRINT("OpenGL context version: %d.%d", majorVersion, minorVersion);
    #endif

    // wglSwapIntervalEXT(0); // disable vsync
    wglSwapIntervalEXT(1); // enable vsync

    glClearColor(0.26f, 0.53f, 0.96f, 1.0f); // cornflower blue

    return true;
}

inline b8 wip_test_init() {
    using namespace internal;

    const f32 vertices[]{
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
    };

    glGenVertexArrays(1, &wip_vao);
    glBindVertexArray(wip_vao);

    glGenBuffers(1, &wip_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, wip_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), nullptr);
    glEnableVertexAttribArray(0);

    const auto vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    {
        const u32 bufferSize{ 4096 };
        c8* buffer[bufferSize] = {};
        u32 fileSize{ 0 };
        if (!filesystem::readEntireFile("shaders/test.vert", reinterpret_cast<u8*>(buffer), bufferSize, fileSize)) { return false; }

        const c8* begin = reinterpret_cast<const c8*>(buffer);
        glShaderSource(vertexShaderId, 1, &begin, nullptr);
        glCompileShader(vertexShaderId);
        i32 success{ 0 };
        const auto infoLogSize{ 512 };
        c8 infoLog[infoLogSize] = {};
        glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShaderId, infoLogSize, nullptr, infoLog);
            E_PRINT("Vertex shader compilation failed: %s", infoLog);
            return false;
        }
    }

    const auto fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    {
        const u32 bufferSize{ 4096 };
        c8* buffer[bufferSize] = {};
        u32 fileSize{ 0 };
        if (!filesystem::readEntireFile("shaders/test.frag", reinterpret_cast<u8*>(buffer), bufferSize, fileSize)) { return false; }

        const c8* begin = reinterpret_cast<const c8*>(buffer);
        glShaderSource(fragmentShaderId, 1, &begin, nullptr);
        glCompileShader(fragmentShaderId);
        i32 success{ 0 };
        const auto infoLogSize{ 512 };
        c8 infoLog[infoLogSize] = {};
        glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShaderId, infoLogSize, nullptr, infoLog);
            E_PRINT("Fragment shader compilation failed: %s", infoLog);
            return false;
        }
    }

    wip_program = glCreateProgram();
    {
        glAttachShader(wip_program, vertexShaderId);
        glAttachShader(wip_program, fragmentShaderId);
        glLinkProgram(wip_program);
        i32 success{ 0 };
        const auto infoLogSize{ 512 };
        c8 infoLog[infoLogSize] = {};
        glGetProgramiv(wip_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(wip_program, infoLogSize, nullptr, infoLog);
            E_PRINT("Shader program linking failed: %s", infoLog);
            return false;
        }
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    glUseProgram(wip_program);

    return true;
}

inline void beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

inline void endFrame() {
    ::SwapBuffers(internal::wglDC);
}

} // namespace base::opengl

#include "win32_opengl.inl"