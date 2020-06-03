#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H

#include "Misc.h"
#include <string>
#include <vector>

struct AtlasInfo
{
    std::string name;
    FloatRect rect;
};

class TextureAtlas final
{
private:
    std::vector<AtlasInfo> atlas;

public:
    TextureAtlas();
    ~TextureAtlas() = default;

    bool LoadAtlas( const std::string &name );
    const std::vector<AtlasInfo> &GetAtlasList() const;
};

inline const std::vector<AtlasInfo> &TextureAtlas::GetAtlasList() const
{
    return atlas;
}

#endif // TEXTUREATLAS_H
