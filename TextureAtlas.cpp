#include "TextureAtlas.h"

#include <sstream>
#include <fstream>

#include "thrid_party/picojson.h"

template <typename T>
T Converter( const picojson::value &val, const std::string &key )
{
    static_assert( std::is_same<T, void>::value, "Converter for this type is not specialized" );
    return T{};
}

template <>
inline float Converter<float>( const picojson::value &val, const std::string &key )
{
    const auto &v = val.get( key );

    if( v.is<picojson::null>() || !v.is<double>() )
    {
        return {};
    }

    return float( v.get<double>() );
}

template <>
inline FloatRect Converter<FloatRect>( const picojson::value &val, const std::string &key )
{
    const auto &v = val.get( key );

    if( v.is<picojson::null>() || !v.is<picojson::object>() )
    {
        return {};
    }

    FloatRect rect;

    rect.x = Converter<float>( v, "X" );
    rect.y = Converter<float>( v, "Y" );
    rect.z = Converter<float>( v, "W" );
    rect.w = Converter<float>( v, "H" );

    return rect;
}

TextureAtlas::TextureAtlas()
{
    atlas.reserve( 24 );
}

bool TextureAtlas::LoadAtlas( const std::string &name )
{
    std::stringstream ss;
    std::ifstream f;

    // Read Json file
    f.open( name, std::ios::binary );
    if( !f.is_open() )
    {
        return 1;
    }
    ss << f.rdbuf();
    f.close();

    // Parse Json data
    picojson::value v;
    ss >> v;
    std::string err = picojson::get_last_error();
    if( !err.empty() )
    {
        std::cerr << err << std::endl;
        return false;
    }

    if( !v.is<picojson::object>() )
    {
        std::cout << "Error read '" << name << "' file\n";
        return false;
    }

    const picojson::object &obj = v.get<picojson::object>();
    for( auto &&[name, ent] : obj )
    {
        if( !ent.is<picojson::object>() )
        {
            std::cout << name << " not a object!\n";
            continue;
        }

        for( auto &&[atlasName, data] : ent.get<picojson::object>() )
        {
            AtlasInfo atlasInfo;

            atlasInfo.name = atlasName;
            atlasInfo.rect = Converter<FloatRect>( ent, atlasName );

            atlas.push_back( atlasInfo );
        }
    }

    return true;
}
