#pragma once

#include <GL/glew.h>
#include <cstring>
#include <memory>
#include "CommonValues.h"
#include <iostream>

class Texture
{
    public:
        Texture();
        Texture(const char* fileLocation);

        const char* getFileLocation();

        bool loadTexture();
        void useTexture();
        void clearTexture();

        ~Texture();

    private:
        GLuint textureID {};
        int width {}, height {};

        const char* fileLocation {};
};
