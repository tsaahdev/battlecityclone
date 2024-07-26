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

inline FramebufferConfig* pickBestPixelFormat(const FramebufferConfig* configs, i32 count) {
    const FramebufferConfig* best{ nullptr };
    const FramebufferConfig* current{ nullptr };
    for (i32 i = 0; i < count; ++i) {
        current = configs + i;

        // TODO:: finish it, but now sleeeeeeeeeeeeeeeeeeeeeep
    }
    return nullptr;
}

inline i32 findPixelFormatAttribValueWGL(const i32* attribs, i32 attribCount, const i32* values, i32 attrib) {
    for (i32 i = 0; i < attribCount; ++i) {
        if (attribs[i] == attrib) { return values[i]; }
    }
    return 0;
}
#define E_FIND_ATTRIB_VALUE(x) findPixelFormatAttribValueWGL(attribs, attribCount, values, x)

inline i32 choosePixelFormat() {
    const i32 attribsCount{ 40 };
    i32 attribs[attribsCount] = {};
    auto nativeCount = ::DescribePixelFormat(windowDc, 1, sizeof(::PIXELFORMATDESCRIPTOR), nullptr);
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
        if (!wglGetPixelFormatAttribivARB(windowDc, 1, 0, 1, &attrib, &extensionCount) || extensionCount == 0) {
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
        if (state.ARB_pixelFormat) { // TODO: check it - seems something is off
            if (!wglGetPixelFormatAttribivARB(windowDc, pixelFormat, 0, attribIndex, attribs, values)) { return 0; }
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
                if (E_FIND_ATTRIB_VALUE(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB)) {
                    u.srgb = true;
                }
            }
        } else {
            ::PIXELFORMATDESCRIPTOR pfd = {};
            if (!::DescribePixelFormat(windowDc, pixelFormat, sizeof(::PIXELFORMATDESCRIPTOR), &pfd)) { return 0; }
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

    const auto wglDC = wglGetCurrentDC();
    const auto wglDirectContext = wglGetCurrentContext();
    if (!wglMakeCurrent(dc, dummyContext)) {
        wglMakeCurrent(wglDC, wglDirectContext);
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

        E_DEBUG_OP(
        c8 buffer[2048];
        for (i32 ei = 0, pi = 0;; ++ei, ++pi) {
            const c8 currentChar = extensions[ei];
            if (currentChar == ' ' && extensions[ei + 1] == '\0') { buffer[pi] = '\0'; break; }
            else if (currentChar == ' ') { buffer[pi] = '\n'; }
            else { buffer[pi] = currentChar; }
        }
        ::OutputDebugStringA(buffer);
        );
    }

    wglMakeCurrent(wglDC, wglDirectContext);
    wglDeleteContext(dummyContext);

    return true;
}

inline b8 createContext(::HWND windowHandle) {
    windowDc = ::GetDC(windowHandle);
    const auto pixelFormat = choosePixelFormat();
    // choose pixel format
    // describe pixel format
    // set pixel format
    // set core mask bit
    // set debug flag if needed
    // set context version attribs
    // set context flags and mask
    // create wgl attribs
    // create context

    return true;
}

} // namespace base::opengl::internal