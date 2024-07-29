#pragma once

#include "common.h"

#include "win32_opengl_defines.h"

#include "e_png.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <unordered_map>


namespace base::opengl {

enum class TextureId: u32 {
    None = 0,
    TestOpaque,
    TileSet,
};
enum class SubTextureId: u32 {
    None = 0,
    Brick,
};

struct SubTexture {
    v2 topLeft{ 0.0f, 0.0f };
    v2 bottomRight{ 0.0f, 0.0f };
    TextureId id{ TextureId::None };
};

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

// i need a location where to store a hash of subtextures.
// what if i have an additional storage for textures to be referenced in my subtextures?

std::unordered_map<TextureId, u32> textureMap;
std::unordered_map<SubTextureId, SubTexture> subTextureMap;



} // namespace internal



inline b8 loadTexture(const c8* filename, TextureId id) {
    u32 textureId{ 0 };
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    i32 x, y, n;
    u8* data = stbi_load(filename, &x, &y, &n, 0);
    if (!data) {
        E_PRINT("Failed to load texture: %s", filename);
        return false;
    }
    if (n == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else if (n == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
        E_PRINT("Unsupported number of channels: %d", n);
        stbi_image_free(data);
        return false;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    internal::textureMap[id] = textureId;
    return true;
}

inline void addSubtexture(TextureId texture, SubTextureId id, v2 topLeft, v2 bottomRight) {
    internal::subTextureMap[id] = SubTexture{ topLeft, bottomRight, texture };
    // TODO: do it in a different way pls, store subtextures in a array so we could pass it into gpu esealy 
    // and also let's store texture with full uv into subtexture aswell and use subtexture everywhere
    // and now i am so tired that i'll pass out ) c ya
}

inline void bindTexture(TextureId id) {
    glBindTexture(GL_TEXTURE_2D, internal::textureMap[id]);
}
inline void bindTexture(SubTextureId id) {
    const auto& subTexture = internal::subTextureMap[id];
    glBindTexture(GL_TEXTURE_2D, internal::textureMap[subTexture.id]);
}

inline void pushQuad(SubTextureId id, v2 possition, v2 size) {

}
u32 wip_vao{ 0 };
u32 wip_vbo{ 0 };
u32 wip_program{ 0 };
u32 wip_texture{ 0 };

inline b8 init(::HINSTANCE instance, ::HWND windowHandle, i32 width, i32 height) {
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

    if (!loadTexture("../assets/test_opaque.png", TextureId::TestOpaque)) { return false; }
    if (!loadTexture("../assets/tileset.jpg", TextureId::TileSet)) { return false; }
    addSubtexture(TextureId::TileSet, SubTextureId::Brick, v2{ 256 / 400.0f, 0.0f }, v2{ (256 + 16) / 400.0f, 16 / 256.0f });

    // wglSwapIntervalEXT(0); // disable vsync
    wglSwapIntervalEXT(1); // enable vsync

    glViewport(0, 0, width, height);
    glClearColor(0.26f, 0.53f, 0.96f, 1.0f); // cornflower blue

    return true;
}



inline void beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(wip_vao);
    glUseProgram(wip_program);
    bindTexture(TextureId::TileSet);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

inline void endFrame() {
    ::SwapBuffers(internal::wglDC);
}

inline void resize(i32 width, i32 height) {
    glViewport(0, 0, width, height);
    beginFrame();
    endFrame();
}


inline b8 wip_test_init() {
    using namespace internal;
    // ok  so i do wwant to draw a bunch of textured quads
    // and i want them to be instanced
    // vertex positions are vertex specific that is obvious
    // so are texture coords, however i am not sure how to use instanced texture coords
    // i mean i can use a texture atlas and then use a texture coord offset
    // but i am not sure how to do it yet

    // ok i guess i just pass instanced texture coords and then in vertex shader calculate the actual texture coords bet quad vertex by just assigning them to instanced values
    // and i think it is only needed if i want to use different subtexture sizes
    // if they were same size every time - i could pass just integer coords and calc uvs much esear

    // i mean i also think can i pass less similar data to instanced data?
    // in theory i am drawinig very discrete images
    // i could go for integers for positions and sizes as well as texture coords    though i am not sure if it is worth it
    // OR A BETTER IDEA
    // what if I prepare atlas description data in a uniform buffer and just reference it by texture index????!!?!?!
    // so i will have to pass 2xu8 for pos 1xu8 for size and 1xu8 for texture index
    // also i could use bindless textures to allow for more textures if needed

    // sounds like a plan
    // - so let's do a static buffer for atlas data on init when all textures are loaded and it will refer to all the textures and subtextures
    // - then pass 2xu8 for pos 1xu8 for size and 1xu8 for texture index as instanced data
    // - then render a quad million times

    struct AtlasEntry {
        SubTexture subTexture;
        SubTextureId id;
    };
    AtlasEntry atlasData[100]{};
    atlasData[static_cast<u32>(SubTextureId::Brick)] = { internal::subTextureMap[SubTextureId::Brick], SubTextureId::Brick };


    const f32 quad[]{
        -0.5f,  0.5f,
         0.5f,  0.5f,
         0.5f, -0.5f,
        -0.5f, -0.5f
    };
    const i16 instanceData[]{ // TODO: think on size better
        // position, size, subtextureId
        0, 0, 1, 1,
        1, 0, 2, 1
    };
    // i am so damn tired i cannot comrehand what i am doing lol
    // i mean i am forgetting whhy i am doing it )


    // const f32 vertices[]{
    //     -0.5f,  0.5f, 0.0f, 0.0f,
    //      0.5f,  0.5f, 1.0f, 0.0f,
    //      0.5f, -0.5f, 1.0f, 1.0f,
    //     -0.5f, -0.5f, 0.0f, 1.0f
    // };
    const f32 vertices[]{
        -0.5f,  0.5f, 256 / 400.0f, 0.0f,
         0.5f,  0.5f, (256 + 16) / 400.0f, 0.0f,
         0.5f, -0.5f, (256 + 16) / 400.0f, 16 / 256.0f,
        -0.5f, -0.5f, 256 / 400.0f, 16 / 256.0f
    };

    glGenVertexArrays(1, &wip_vao);
    glBindVertexArray(wip_vao);

    glGenBuffers(1, &wip_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, wip_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), reinterpret_cast<void*>(2 * sizeof(f32)));
    glEnableVertexAttribArray(1);

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

    

    glUniform1i(glGetUniformLocation(wip_program, "texture0"), 0);

    return true;
}

} // namespace base::opengl

