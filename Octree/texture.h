// Texture Class    by Alan Baylis 2001

#ifndef TextureH
#define TextureH

#include <windows.h>
#include "main.h"

class TEXTURE
{
public:
    TEXTURE();
    ~TEXTURE();

    bool LoadTGA();
    bool LoadTGA(GLenum MagFilter, GLenum MinFilter, GLenum WrapS, GLenum WrapT, bool MipMap);

    char TexName[256];
    GLubyte* ImageData;  // Image Data (Up To 32 Bits)
    GLuint  Bpp;         // Image Color Depth In Bits Per Pixel
    GLuint  Width;       // Image Width
    GLuint  Height;      // Image Height
    GLuint  TexID;       // Texture ID Used To Select A Texture
    float Vertex1_x;
    float Vertex1_y;
    float Vertex2_x;
    float Vertex2_y;
    float Vertex3_x;
    float Vertex3_y;
};

#endif

