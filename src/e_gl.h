#pragma once

#include "common.h"

namespace base::opengl {

const i32 GL_COLOR_BUFFER_BIT = 0x00004000;
const i32 GL_MAJOR_VERSION = 0x821b;
const i32 GL_MINOR_VERSION = 0x821c;
const i32 GL_VERSION = 0x1f02;

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
static void (__stdcall* glCreateShader)(u32){ nullptr };
static void (__stdcall* glShaderSource)(u32, i32, const c8**, const i32*){ nullptr };
static void (__stdcall* glCompileShader)(u32){ nullptr };
static void (__stdcall* glGetShaderiv)(u32, u32, i32*){ nullptr };
static void (__stdcall* glGetShaderInfoLog)(u32, i32, i32*, c8*){ nullptr };
static void (__stdcall* glDeleteShader)(u32){ nullptr };
static void (__stdcall* glCreateProgram)(){ nullptr };
static void (__stdcall* glAttachShader)(u32, u32){ nullptr };
static void (__stdcall* glLinkProgram)(u32){ nullptr };
static void (__stdcall* glGetProgramiv)(u32, u32, i32*){ nullptr };
static void (__stdcall* glGetProgramInfoLog)(u32, i32, i32*, c8*){ nullptr };
static void (__stdcall* glUseProgram)(u32){ nullptr };
static void (__stdcall* glDeleteProgram)(u32){ nullptr };
static void (__stdcall* glDrawArrays)(u32, i32, i32){ nullptr };

} // namespace base::opengl