#pragma once

#include "common.h"

#include "win32_opengl_defines.h"

#include "stb_image.h"

namespace base::opengl {

struct Texture2D {
    b8 create(u32 aWidth, u32 aHeight, u32 aChannels, const u8* data) {
        if (!data) {
            E_PRINT("Textrue2D::create No data provided");
            return false;
        }
        if (!(aChannels == 3 || aChannels == 4)) {
            E_PRINT("Unsupported number of channels: %d", height);
            return false;
        }
        width = aWidth;
        height = aHeight;
        channels = aChannels;

        glGenTextures(1, &id);
        bind();

        if (channels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (channels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        unbind();
        return true;
    }
    b8 destroy() {
        glDeleteTextures(1, &id);
    }
    void bind() {
        glBindTexture(GL_TEXTURE_2D, id);
    }
    static void unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    u32 id{ 0 };
    u32 width{ 0 };
    u32 height{ 0 };
    u32 channels{ 0 };
};

} // namespace base::opengl