#pragma once

#include <Windows.h>

#include "common.h"

namespace base::opengl {

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
