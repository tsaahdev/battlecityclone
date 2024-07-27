#pragma once

#include <Windows.h>

#include "common.h"
#include "e_gl.h"

namespace base::opengl {

// profile bits
const i32 WGL_CONTEXT_CORE_PROFILE_BIT_ARB = 0x00000001;
const i32 WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB = 0x00000002;

// flags
const i32 WGL_CONTEXT_DEBUG_BIT_ARB = 0x0001;
const i32 WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB = 0x0002;
const i32 WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB = 0x0004;

// context robustness strategy
const i32 WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB = 0x8256;
const i32 WGL_LOSE_CONTEXT_ON_RESET_ARB = 0x8252;
const i32 WGL_NO_RESET_NOTIFICATION_ARB = 0x8261;

// context release behavior
const i32 WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB = 0;
const i32 WGL_CONTEXT_RELEASE_BEHAVIOR_ARB = 0x2097;
const i32 WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB = 0x2098;

// context no error
const i32 WGL_CONTEXT_OPENGL_NO_ERROR_ARB = 0x31b3;

const i32 WGL_CONTEXT_MAJOR_VERSION_ARB = 0x2091;
const i32 WGL_CONTEXT_MINOR_VERSION_ARB = 0x2092;

const i32 WGL_CONTEXT_FLAGS_ARB = 0x2094;
const i32 WGL_CONTEXT_PROFILE_MASK_ARB = 0x9126;

const i32 WGL_NUMBER_PIXEL_FORMATS_ARB = 0x2000;
const i32 WGL_DRAW_TO_WINDOW_ARB = 0x2001;
const i32 WGL_ACCELERATION_ARB = 0x2003;
const i32 WGL_SUPPORT_OPENGL_ARB = 0x2010;
const i32 WGL_DOUBLE_BUFFER_ARB = 0x2011;
const i32 WGL_PIXEL_TYPE_ARB = 0x2013;
const i32 WGL_RED_BITS_ARB = 0x2015;
const i32 WGL_RED_SHIFT_ARB = 0x2016;
const i32 WGL_GREEN_BITS_ARB = 0x2017;
const i32 WGL_GREEN_SHIFT_ARB = 0x2018;
const i32 WGL_BLUE_BITS_ARB = 0x2019;
const i32 WGL_BLUE_SHIFT_ARB = 0x201a;
const i32 WGL_ALPHA_BITS_ARB = 0x201b;
const i32 WGL_ALPHA_SHIFT_ARB = 0x201c;
const i32 WGL_DEPTH_BITS_ARB = 0x2022;
const i32 WGL_STENCIL_BITS_ARB = 0x2023;
const i32 WGL_TYPE_RGBA_ARB = 0x202b;
const i32 WGL_SAMPLES_ARB = 0x2042;
const i32 WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB = 0x20a9;

static ::HGLRC(__stdcall* wglCreateContext)(::HDC){ nullptr };
static b32(__stdcall* wglDeleteContext)(::HGLRC){ nullptr };
static ::PROC(__stdcall* wglGetProcAddress)(const c8*){ nullptr };
static ::HGLRC(__stdcall* wglGetCurrentContext)(){ nullptr };
static ::HDC(__stdcall* wglGetCurrentDC)(){ nullptr };
static b32(__stdcall* wglMakeCurrent)(::HDC, ::HGLRC){ nullptr };
static b32(__stdcall* wglShareLists)(::HGLRC, ::HGLRC){ nullptr };

static const c8* (__stdcall* wglGetExtensionsStringEXT)(){ nullptr };
static const c8* (__stdcall* wglGetExtensionsStringARB)(::HDC){ nullptr };
static ::HGLRC (__stdcall* wglCreateContextAttribsARB)(::HDC, ::HGLRC, const i32*){ nullptr };
static b32 (__stdcall* wglSwapIntervalEXT)(i32){ nullptr };
static b32 (__stdcall* wglGetPixelFormatAttribivARB)(::HDC, i32, i32, u32, const i32*, i32*){ nullptr };

} // namespace base::opengl
