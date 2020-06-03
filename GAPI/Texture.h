#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
private:
    unsigned int texture = 0;

    unsigned int width = 0;
    unsigned int height = 0;

public:
    Texture() = default;
    ~Texture();

    bool LoadFromMemory( const void *data, const unsigned int w, const unsigned int h, const bool smooth,
                         const bool genMipMaps );

    unsigned int GetTexture() const;

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

    bool IsLoaded() const;

    void Bind( const unsigned int slot );
    void Unbind();
};

inline unsigned int Texture::GetTexture() const
{
    return texture;
}

inline unsigned int Texture::GetWidth() const
{
    return width;
}

inline unsigned int Texture::GetHeight() const
{
    return height;
}

#endif // TEXTURE_H
