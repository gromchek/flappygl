#include "Texture.h"
#include <GL/glew.h>

Texture::~Texture()
{
    glDeleteTextures( 1, &texture );
}

bool Texture::LoadFromMemory( const void *data, const unsigned int w, const unsigned int h, const bool smooth,
                              const bool genMipMaps )
{
    if( IsLoaded() )
    {
        return false;
    }

    glGenTextures( 1, &texture );

    glBindTexture( GL_TEXTURE_2D, texture );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, int( w ), int( h ), 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST );

    width = w;
    height = h;

    if( genMipMaps )
    {
        glGenerateMipmap( GL_TEXTURE_2D );
    }
    else
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
    }

    return true;
}

bool Texture::IsLoaded() const
{
    return glIsTexture( texture );
}

void Texture::Bind( const unsigned int slot )
{
    glActiveTexture( GL_TEXTURE0 + slot );
    glBindTexture( GL_TEXTURE_2D, texture );
}

void Texture::Unbind()
{
    glBindTexture( GL_TEXTURE_2D, 0 );
}
