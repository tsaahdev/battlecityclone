#pragma once

#include "common.h"

namespace base::opengl {

const i32 GL_COLOR_BUFFER_BIT = 0x00004000;
const i32 GL_MAJOR_VERSION = 0x821b;
const i32 GL_MINOR_VERSION = 0x821c;
const i32 GL_VENDOR = 0x1f00;
const i32 GL_RENDERER = 0x1f01;
const i32 GL_VERSION = 0x1f02;
const i32 GL_SHADING_LANGUAGE_VERSION = 0x8b8c;

const i32 GL_DEBUG_SEVERITY_NOTIFICATION = 0x826b;
const i32 GL_DEBUG_SOURCE_API = 0x8246;
const i32 GL_DEBUG_SOURCE_WINDOW_SYSTEM = 0x8247;
const i32 GL_DEBUG_SOURCE_SHADER_COMPILER = 0x8248;
const i32 GL_DEBUG_SOURCE_THIRD_PARTY = 0x8249;
const i32 GL_DEBUG_SOURCE_APPLICATION = 0x824a;
const i32 GL_DEBUG_SOURCE_OTHER = 0x824b;
const i32 GL_DEBUG_TYPE_ERROR = 0x824c;
const i32 GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR = 0x824d;
const i32 GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR = 0x824e;
const i32 GL_DEBUG_TYPE_PORTABILITY = 0x824f;
const i32 GL_DEBUG_TYPE_PERFORMANCE = 0x8250;
const i32 GL_DEBUG_TYPE_OTHER = 0x8251;
const i32 GL_DEBUG_OUTPUT = 0x92e0;

const i32 GL_VERTEX_SHADER = 0x8b31;
const i32 GL_FRAGMENT_SHADER = 0x8b30;
const i32 GL_COMPILE_STATUS = 0x8b81;
const i32 GL_LINK_STATUS = 0x8b82;

const i32 GL_ARRAY_BUFFER = 0x8892;
const i32 GL_UNIFORM_BUFFER = 0x8a11;
const i32 GL_STATIC_DRAW = 0x88e4;

const i32 GL_FLOAT = 0x1406;
const i32 GL_INT = 0x1404;
const i32 GL_FALSE = 0;

const i32 GL_TRIANGLE_FAN = 0x0006;

const i32 GL_TEXTURE_2D = 0x0de1;
const i32 GL_TEXTURE_WRAP_S = 0x2802;
const i32 GL_TEXTURE_WRAP_T = 0x2803;
const i32 GL_TEXTURE_MIN_FILTER = 0x2801;
const i32 GL_TEXTURE_MAG_FILTER = 0x2800;
const i32 GL_CLAMP_TO_EDGE = 0x812f;
const i32 GL_REPEAT = 0x2901;
const i32 GL_LINEAR = 0x2601;
const i32 GL_NEAREST = 0x2600;
const i32 GL_RGBA = 0x1908;
const i32 GL_RGB = 0x1907;
const i32 GL_UNSIGNED_BYTE = 0x1401;



static void (__stdcall* glClear)(u32){ nullptr };
static void (__stdcall* glClearColor)(f32, f32, f32, f32){ nullptr };
static void (__stdcall* glViewport)(i32, i32, i32, i32){ nullptr };
static const c8* (__stdcall* glGetString)(u32){ nullptr };
static void (__stdcall* glGetIntegerv)(u32, i32*){ nullptr };
static void (__stdcall* glEnable)(u32){ nullptr };
static void (__stdcall* glDisable)(u32){ nullptr };
static void (__stdcall* glDebugMessageCallback)(void(__stdcall*)(u32, u32, u32, u32, i32, const c8*, void*), void*){ nullptr };
static void (__stdcall* glGenVertexArrays)(i32, u32*){ nullptr };
static void (__stdcall* glBindVertexArray)(u32){ nullptr };
static void (__stdcall* glDeleteVertexArrays)(i32, const u32*){ nullptr };
static void (__stdcall* glGenBuffers)(i32, u32*){ nullptr };
static void (__stdcall* glBindBuffer)(u32, u32){ nullptr };
static void (__stdcall* glBufferData)(u32, i32, const void*, u32){ nullptr };
static void (__stdcall* glDeleteBuffers)(i32, const u32*){ nullptr };
static void (__stdcall* glVertexAttribPointer)(u32, i32, u32, b8, i32, const void*){ nullptr };
static void (__stdcall* glEnableVertexAttribArray)(u32){ nullptr };
static void (__stdcall* glDisableVertexAttribArray)(u32){ nullptr };
static u32 (__stdcall* glCreateShader)(u32){ nullptr };
static void (__stdcall* glShaderSource)(u32, i32, const c8**, const i32*){ nullptr };
static void (__stdcall* glCompileShader)(u32){ nullptr };
static void (__stdcall* glGetShaderiv)(u32, u32, i32*){ nullptr };
static void (__stdcall* glGetShaderInfoLog)(u32, i32, i32*, c8*){ nullptr };
static void (__stdcall* glDeleteShader)(u32){ nullptr };
static u32 (__stdcall* glCreateProgram)(){ nullptr };
static void (__stdcall* glAttachShader)(u32, u32){ nullptr };
static void (__stdcall* glLinkProgram)(u32){ nullptr };
static void (__stdcall* glGetProgramiv)(u32, u32, i32*){ nullptr };
static void (__stdcall* glGetProgramInfoLog)(u32, i32, i32*, c8*){ nullptr };
static void (__stdcall* glUseProgram)(u32){ nullptr };
static void (__stdcall* glDeleteProgram)(u32){ nullptr };
static void (__stdcall* glDrawArrays)(u32, i32, i32){ nullptr };
static void (__stdcall* glGenTextures)(i32, u32*){ nullptr };
static void (__stdcall* glBindTexture)(u32, u32){ nullptr };
static void (__stdcall* glTexImage2D)(u32, i32, i32, i32, i32, i32, u32, u32, const void*){ nullptr };
static void (__stdcall* glGenerateMipmap)(u32){ nullptr };
static void (__stdcall* glTexParameteri)(u32, u32, i32){ nullptr };
static i32 (__stdcall* glGetUniformLocation)(u32, const c8*){ nullptr };
static void (__stdcall* glUniform1i)(i32, i32){ nullptr };
static void (__stdcall* glBindBufferBase)(u32, u32, u32){ nullptr };
static u32 (__stdcall* glGetUniformBlockIndex)(u32, const c8*){ nullptr };
static void (__stdcall* glUniformBlockBinding)(u32, u32, u32){ nullptr };
static void (__stdcall* glBindBufferRange)(u32, u32, u32, i32, i32){ nullptr };
static void (__stdcall* glVertexAttribDivisor)(u32, u32){ nullptr };
static void (__stdcall* glDrawArraysInstanced)(u32, i32, i32, i32){ nullptr };

} // namespace base::opengl