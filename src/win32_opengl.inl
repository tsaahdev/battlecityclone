#include "e_gl.h"
#include "win32_opengl.h"

namespace base::opengl::internal {

template <class FPTR>
inline b8 loadFunction(FPTR& var, const c8* name) {
    if (!library) { return false; }
    if (wglGetProcAddress) {
        var = reinterpret_cast<FPTR>(wglGetProcAddress(name));
        if (var) { return true; }
    }
    var = reinterpret_cast<FPTR>(::GetProcAddress(library, name));
    return var != nullptr;
}

#define E_LOAD_FUNCTION(name) loadFunction(name, #name)

inline b8 stringExistsInExtensions(const c8* extensions, const c8* string) {
    const c8* start = extensions;
    for (;;) {
        start = strstr(start, string);
        if (!start) { return false; }
        if (start == extensions || start[-1] == ' ') {
            const c8 next = start[strlen(string)];
            if (next == ' ' || next == '\0') { return true; }
        }
        start += strlen(string);
    }
    return false;
}


struct FramebufferConfig {
    i32 redBits{ 8 };
    i32 greenBits{ 8 };
    i32 blueBits{ 8 };
    i32 alphaBits{ -1 };
    i32 depthBits{ -1 };
    i32 stencilBits{ -1 };
    i32 samples{ -1 };
    i32 handle{ 0 };
    b8 srgb{ false };
};

inline const FramebufferConfig* pickBestPixelFormat(const FramebufferConfig* configs, i32 count) {
    const FramebufferConfig* best{ nullptr };
    const FramebufferConfig* current{ nullptr };

    const i32 desiredRedBits = 8;
    const i32 desiredGreenBits = 8;
    const i32 desiredBlueBits = 8;
    const i32 desiredAlpaBits = 8;
    const i32 desiredDepthBits = 8;
    const i32 desiredStencilBits = 8;
    const i32 desiredSamples = -1;
    const b8 desiredSrgb = true;

    u32 colorDiff{ 0 };
    u32 extraDiff{ 0 };
    u32 missing{ 0 };
    u32 leastMissing{ U32_MAX };
    u32 leastColorDiff{ U32_MAX };
    u32 leastExtraDiff{ U32_MAX };
    for (i32 i = 0; i < count; ++i) {
        current = configs + i;

        { // count number of missing buffers
            if (desiredAlpaBits > 0 && current->alphaBits == 0) { ++missing; }
            if (desiredDepthBits > 0 && current->depthBits == 0) { ++missing; }
            if (desiredStencilBits > 0 && current->stencilBits == 0) { ++missing; }
            if (desiredSamples > 0 && current->samples == 0) { ++missing; }
        }

        { // calc color channel size difference
            colorDiff = 0;
            if (desiredRedBits > 0) { colorDiff += (desiredRedBits - current->redBits) * (desiredRedBits - current->redBits); }
            if (desiredGreenBits > 0) { colorDiff += (desiredGreenBits - current->greenBits) * (desiredGreenBits - current->greenBits); }
            if (desiredBlueBits > 0) { colorDiff += (desiredBlueBits - current->blueBits) * (desiredBlueBits - current->blueBits); }
        }

        { // calc non color channel size diff
            extraDiff = 0;
            if (desiredAlpaBits > 0) { extraDiff += (desiredAlpaBits - current->alphaBits) * (desiredAlpaBits - current->alphaBits); }
            if (desiredDepthBits > 0) { extraDiff += (desiredDepthBits - current->depthBits) * (desiredDepthBits - current->depthBits); }
            if (desiredStencilBits > 0) { extraDiff += (desiredStencilBits - current->stencilBits) * (desiredStencilBits - current->stencilBits); }
            if (desiredSamples > 0) { extraDiff += (desiredSamples - current->samples) * (desiredSamples - current->samples); }
            if (desiredSrgb != current->srgb) { ++extraDiff; }
        }

        if (missing < leastMissing) {
            best = current;
        } else if (missing == leastMissing) {
            if ((colorDiff < leastColorDiff) || (colorDiff == leastColorDiff && extraDiff < leastExtraDiff)) {
                best = current;
            }
        }

        if (current == best) {
            leastMissing = missing;
            leastColorDiff = colorDiff;
            leastExtraDiff = extraDiff;
        }
    }
    return best;
}

inline i32 findPixelFormatAttribValueWGL(const i32* attribs, i32 attribCount, const i32* values, i32 attrib) {
    for (i32 i = 0; i < attribCount; ++i) {
        if (attribs[i] == attrib) { return values[i]; }
    }
    return 0;
}
#define E_FIND_ATTRIB_VALUE(x) findPixelFormatAttribValueWGL(attribs, attribIndex, values, x)

inline i32 choosePixelFormat() {
    const i32 attribsCount{ 40 };
    i32 attribs[attribsCount] = {};
    auto nativeCount = ::DescribePixelFormat(wglDC, 1, sizeof(::PIXELFORMATDESCRIPTOR), nullptr);
    u8 attribIndex{ 0 };
    if (state.ARB_pixelFormat) {
        attribs[attribIndex++] = WGL_SUPPORT_OPENGL_ARB;
        attribs[attribIndex++] = WGL_DRAW_TO_WINDOW_ARB;
        attribs[attribIndex++] = WGL_PIXEL_TYPE_ARB;
        attribs[attribIndex++] = WGL_ACCELERATION_ARB;
        attribs[attribIndex++] = WGL_RED_BITS_ARB;
        attribs[attribIndex++] = WGL_RED_SHIFT_ARB;
        attribs[attribIndex++] = WGL_GREEN_BITS_ARB;
        attribs[attribIndex++] = WGL_GREEN_SHIFT_ARB;
        attribs[attribIndex++] = WGL_BLUE_BITS_ARB;
        attribs[attribIndex++] = WGL_BLUE_SHIFT_ARB;
        attribs[attribIndex++] = WGL_ALPHA_BITS_ARB;
        attribs[attribIndex++] = WGL_ALPHA_SHIFT_ARB;
        attribs[attribIndex++] = WGL_DEPTH_BITS_ARB;
        attribs[attribIndex++] = WGL_STENCIL_BITS_ARB;
        attribs[attribIndex++] = WGL_DOUBLE_BUFFER_ARB;
        if (state.ARB_multisample) {
            attribs[attribIndex++] = WGL_SAMPLES_ARB;
        }
        if (state.ARB_framebuffer_srgb || state.EXT_framebuffer_srgb) {
            attribs[attribIndex++] = WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB;
        }
        const i32 attrib{ WGL_NUMBER_PIXEL_FORMATS_ARB };
        i32 extensionCount{ 0 };
        if (!wglGetPixelFormatAttribivARB(wglDC, 1, 0, 1, &attrib, &extensionCount) || extensionCount == 0) {
            return 0;
        }
        nativeCount = E_MIN(nativeCount, extensionCount);
    }

    const u32 maxConfigs{ 2048 };
    E_ASSERT(nativeCount <= maxConfigs);
    FramebufferConfig usableConfigs[maxConfigs] = {};


    i32 attribCount{ 0 };
    i32 values[attribsCount] = {};
    i32 pixelFormat{ 0 };
    i32 usableCount{ 0 };
    for (i32 i = 0; i < nativeCount; ++i) {
        FramebufferConfig& u = usableConfigs[usableCount];
        pixelFormat = i + 1;
        if (state.ARB_pixelFormat) {
            if (!wglGetPixelFormatAttribivARB(wglDC, pixelFormat, 0, attribIndex, attribs, values)) { return 0; }
            if (!E_FIND_ATTRIB_VALUE(WGL_SUPPORT_OPENGL_ARB) || !E_FIND_ATTRIB_VALUE(WGL_DRAW_TO_WINDOW_ARB)) { continue; }
            if (E_FIND_ATTRIB_VALUE(WGL_PIXEL_TYPE_ARB) != WGL_TYPE_RGBA_ARB) { continue; }
            if (E_FIND_ATTRIB_VALUE(WGL_ACCELERATION_ARB) == WGL_ACCELERATION_ARB) { continue; }
            if (!E_FIND_ATTRIB_VALUE(WGL_DOUBLE_BUFFER_ARB)) { continue; }

            u.redBits = E_FIND_ATTRIB_VALUE(WGL_RED_BITS_ARB);
            u.greenBits = E_FIND_ATTRIB_VALUE(WGL_GREEN_BITS_ARB);
            u.blueBits = E_FIND_ATTRIB_VALUE(WGL_BLUE_BITS_ARB);
            u.alphaBits = E_FIND_ATTRIB_VALUE(WGL_ALPHA_BITS_ARB);
            u.depthBits = E_FIND_ATTRIB_VALUE(WGL_DEPTH_BITS_ARB);
            u.stencilBits = E_FIND_ATTRIB_VALUE(WGL_STENCIL_BITS_ARB);

            if (state.ARB_multisample) {
                u.samples = E_FIND_ATTRIB_VALUE(WGL_SAMPLES_ARB);
            }

            if (state.ARB_framebuffer_srgb || state.EXT_framebuffer_srgb) {
                u.srgb = E_FIND_ATTRIB_VALUE(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB) == true;
            }
        } else {
            ::PIXELFORMATDESCRIPTOR pfd = {};
            if (!::DescribePixelFormat(wglDC, pixelFormat, sizeof(::PIXELFORMATDESCRIPTOR), &pfd)) { return 0; }
            if (!(pfd.dwFlags & PFD_SUPPORT_OPENGL) || !(pfd.dwFlags & PFD_DRAW_TO_WINDOW)) { continue; }
            if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED) && (pfd.dwFlags & PFD_GENERIC_FORMAT)) { continue; }
            if (pfd.iPixelType != PFD_TYPE_RGBA) { continue; }
            if (!!(pfd.dwFlags & PFD_DOUBLEBUFFER) != true) { continue; }

            u.redBits = pfd.cRedBits;
            u.greenBits = pfd.cGreenBits;
            u.blueBits = pfd.cBlueBits;
            u.alphaBits = pfd.cAlphaBits;
            u.depthBits = pfd.cDepthBits;
            u.stencilBits = pfd.cStencilBits;
        }
        u.handle = pixelFormat;
        ++usableCount;
    }

    if (usableCount == 0) { return 0; }

    const auto bestConfig = pickBestPixelFormat(usableConfigs, usableCount);
    if (!bestConfig) { return 0; }

    return bestConfig->handle;
}
#undef E_FIND_ATTRIB_VALUE

inline b8 createHelperWindow(::HINSTANCE instance) {
    const ::WNDCLASSEXA wc{
        .cbSize = sizeof(::WNDCLASSEXA),
        .style = CS_OWNDC,
        .lpfnWndProc = ::DefWindowProcA,
        .hInstance = instance,
        .lpszClassName = "BattleCityOpenglHelperWindowClass",
    };
    if (::RegisterClassExA(&wc) == 0) { return false; }

    helperWindowHandle = ::CreateWindowExA(
        WS_EX_OVERLAPPEDWINDOW,
        "BattleCityOpenglHelperWindowClass",
        "Battle City Opengl Helper Window",
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, 1, 1,
        nullptr, nullptr, instance, nullptr);
    if (!helperWindowHandle) { return false; }

    ::ShowWindow(helperWindowHandle, SW_HIDE);

    ::MSG msg;
    while (::PeekMessageA(&msg, helperWindowHandle, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessageA(&msg);
    }

    return true;
}

inline b8 initWgl() {
    library = ::LoadLibraryA("opengl32.dll");
    if (!library) { return false; }

    { b8 result{ true };
        result |= E_LOAD_FUNCTION(wglCreateContext);
        result |= E_LOAD_FUNCTION(wglDeleteContext);
        result |= E_LOAD_FUNCTION(wglGetProcAddress);
        result |= E_LOAD_FUNCTION(wglGetCurrentContext);
        result |= E_LOAD_FUNCTION(wglGetCurrentDC);
        result |= E_LOAD_FUNCTION(wglMakeCurrent);
        result |= E_LOAD_FUNCTION(wglShareLists);
        if (!result) { return false; }
    }

    const auto dc = ::GetDC(helperWindowHandle);
    if (!dc) { return false; }

    const ::PIXELFORMATDESCRIPTOR pixelFormatDescriptor{
        .nSize = sizeof(::PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 24,
    };

    if (!::SetPixelFormat(dc, ::ChoosePixelFormat(dc, &pixelFormatDescriptor), &pixelFormatDescriptor)) { return false; }

    const auto dummyContext = wglCreateContext(dc);
    if (!dummyContext) { return false; }

    const auto currentWglDC = wglGetCurrentDC();
    const auto currentWglContext = wglGetCurrentContext();
    if (!wglMakeCurrent(dc, dummyContext)) {
        wglMakeCurrent(currentWglDC, currentWglContext);
        wglDeleteContext(dummyContext);
        return false;
    }

    { b8 result{ true };
        result |= (
            E_LOAD_FUNCTION(wglGetExtensionsStringEXT) ||
            E_LOAD_FUNCTION(wglGetExtensionsStringARB)
        );
        result |= E_LOAD_FUNCTION(wglCreateContextAttribsARB);
        result |= E_LOAD_FUNCTION(wglSwapIntervalEXT);
        result |= E_LOAD_FUNCTION(wglGetPixelFormatAttribivARB);
        if (!result) { return false; }
    }

    {
        const c8* extensions{ nullptr };
        if (wglGetExtensionsStringARB) {
            extensions = wglGetExtensionsStringARB(wglDC);
        } else if (wglGetExtensionsStringEXT) {
            extensions = wglGetExtensionsStringEXT();
        }

        state.ARB_pixelFormat = stringExistsInExtensions(extensions, "WGL_ARB_pixel_format");
        state.ARB_multisample = stringExistsInExtensions(extensions, "WGL_ARB_multisample");
        state.ARB_framebuffer_srgb = stringExistsInExtensions(extensions, "WGL_ARB_framebuffer_srgb");
        state.EXT_framebuffer_srgb = stringExistsInExtensions(extensions, "WGL_EXT_framebuffer_srgb");
        state.ARB_create_context = stringExistsInExtensions(extensions, "WGL_ARB_create_context");
        state.ARB_create_context_profile = stringExistsInExtensions(extensions, "WGL_ARB_create_context_profile");
        state.ARB_create_context_robustness = stringExistsInExtensions(extensions, "WGL_ARB_create_context_robustness");
        state.ARB_context_flush_cntrol = stringExistsInExtensions(extensions, "WGL_ARB_context_flush_control");
        state.ARB_context_no_error = stringExistsInExtensions(extensions, "WGL_ARB_context_no_error");

        #if defined(E_DEBUG_BUILD)
        c8 buffer[2048];
        for (i32 ei = 0, pi = 0;; ++ei, ++pi) {
            const c8 currentChar = extensions[ei];
            if (currentChar == ' ' && extensions[ei + 1] == '\0') { buffer[pi++] = '\n'; buffer[pi] = '\0'; break; }
            else if (currentChar == ' ') { buffer[pi] = '\n'; }
            else { buffer[pi] = currentChar; }
        }
        ::OutputDebugStringA(buffer);
        #endif
    }

    wglMakeCurrent(wglDC, currentWglContext);
    wglDeleteContext(dummyContext);

    return true;
}

inline b8 createContext(::HWND windowHandle) {
    wglDC = ::GetDC(windowHandle);
    const auto pixelFormat = choosePixelFormat();

    ::PIXELFORMATDESCRIPTOR pfd = {};
    if (!::DescribePixelFormat(wglDC, pixelFormat, sizeof(::PIXELFORMATDESCRIPTOR), &pfd)) { return false; }
    if (!::SetPixelFormat(wglDC, pixelFormat, &pfd)) { return false; }

    // https://www.khronos.org/opengl/wiki/OpenGL_Context#:~:text=Forward%20compatibility%5Bedit%5D,with%20core%20and%20compatibility%20contexts.
    // Recommendation: You should use the forward compatibility bit only if you need compatibility with MacOS. That API requires the forward compatibility bit to create any core profile context.
    const b8 forwardCompatibleContext{ true };
    if (forwardCompatibleContext && !state.ARB_create_context) { return false; }

    const b8 coreProfileContext{ true };
    if (coreProfileContext && !state.ARB_create_context_profile) { return false; }

    i32 attribs[40] = {};
    i32 attribIndex{ 0 };
    if (state.ARB_create_context) {
        i32 flags{ 0 };
        if (forwardCompatibleContext) { flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB; }
        if (state.ARB_create_context_robustness) {
            attribs[attribIndex++] = WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB;
            attribs[attribIndex++] = WGL_NO_RESET_NOTIFICATION_ARB;
            flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
        }
        if (state.ARB_context_flush_cntrol) {
            attribs[attribIndex++] = WGL_CONTEXT_RELEASE_BEHAVIOR_ARB;
            attribs[attribIndex++] = WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB;
        }
        if (state.ARB_context_no_error) {
            attribs[attribIndex++] = WGL_CONTEXT_OPENGL_NO_ERROR_ARB;
            attribs[attribIndex++] = true;
        }
        attribs[attribIndex++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
        attribs[attribIndex++] = 4;
        attribs[attribIndex++] = WGL_CONTEXT_MINOR_VERSION_ARB;
        attribs[attribIndex++] = 6;

        #if defined(E_DEBUG_BUILD)
            flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
        #endif

        if (flags) {
            attribs[attribIndex++] = WGL_CONTEXT_FLAGS_ARB;
            attribs[attribIndex++] = flags;
        }

        i32 mask{ 0 };
        if (coreProfileContext) {
            mask = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
        } else {
            mask = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
        }

        if (mask) {
            attribs[attribIndex++] = WGL_CONTEXT_PROFILE_MASK_ARB;
            attribs[attribIndex++] = mask;
        }

        wglHandle = wglCreateContextAttribsARB(wglDC, 0, attribs);
        if (!wglHandle) { return false; } // TODO: handle error in order to tune the attribs?
    } else {
        // we need context to draw to otherwise fail
        return false;
    }

    return true;
}

inline b8 loadOpenGlFunctions() {
    b8 result{ true };
    result |= E_LOAD_FUNCTION(glClear);
    result |= E_LOAD_FUNCTION(glClearColor);
    result |= E_LOAD_FUNCTION(glViewport);
    result |= E_LOAD_FUNCTION(glGetString);
    result |= E_LOAD_FUNCTION(glGetIntegerv);
    result |= E_LOAD_FUNCTION(glEnable);
    result |= E_LOAD_FUNCTION(glDisable);
    result |= E_LOAD_FUNCTION(glDebugMessageCallback);
    result |= E_LOAD_FUNCTION(glGenVertexArrays);
    result |= E_LOAD_FUNCTION(glBindVertexArray);
    result |= E_LOAD_FUNCTION(glDeleteVertexArrays);
    result |= E_LOAD_FUNCTION(glGenBuffers);
    result |= E_LOAD_FUNCTION(glBindBuffer);
    result |= E_LOAD_FUNCTION(glBufferData);
    result |= E_LOAD_FUNCTION(glDeleteBuffers);
    result |= E_LOAD_FUNCTION(glVertexAttribPointer);
    result |= E_LOAD_FUNCTION(glEnableVertexAttribArray);
    result |= E_LOAD_FUNCTION(glDisableVertexAttribArray);
    result |= E_LOAD_FUNCTION(glCreateShader);
    result |= E_LOAD_FUNCTION(glShaderSource);
    result |= E_LOAD_FUNCTION(glCompileShader);
    result |= E_LOAD_FUNCTION(glGetShaderiv);
    result |= E_LOAD_FUNCTION(glGetShaderInfoLog);
    result |= E_LOAD_FUNCTION(glDeleteShader);
    result |= E_LOAD_FUNCTION(glCreateProgram);
    result |= E_LOAD_FUNCTION(glAttachShader);
    result |= E_LOAD_FUNCTION(glLinkProgram);
    result |= E_LOAD_FUNCTION(glGetProgramiv);
    result |= E_LOAD_FUNCTION(glGetProgramInfoLog);
    result |= E_LOAD_FUNCTION(glUseProgram);
    result |= E_LOAD_FUNCTION(glDeleteProgram);
    result |= E_LOAD_FUNCTION(glDrawArrays);
    return result;
}

} // namespace base::opengl::internal