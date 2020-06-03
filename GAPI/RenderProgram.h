#ifndef RENDERPROGRAM_H
#define RENDERPROGRAM_H

#include <string>
#include <string_view>

#include "../Misc.h"

class RenderProgram final
{
private:
    static const int MAX_UNIFORMS = 8;

    unsigned int shaderProgram;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    int texLoc[8];

    struct Uniform
    {
        std::string Name;
        int Location;
        int Size;
        int Type;
        Uniform() : Location( 0 ), Size( 0 ), Type( 0 )
        {
        }
    };

    Uniform UniformInfo[MAX_UNIFORMS];


    void loadUniforms();
    bool link();
    bool validate();
    unsigned int loadGLSLShader( const std::string &content, unsigned int type );

    bool setUniformf( std::string_view name, const float *v );

public:
    static const unsigned int INVALID_PROGID = 0xFFFFFFFF;

    RenderProgram();
    ~RenderProgram();

    RenderProgram( const RenderProgram &renProg ) = delete;
    RenderProgram &operator=( const RenderProgram &renProg ) = delete;

    unsigned int GetProgram() const;

    bool CreateShaderProgramRaw( const std::string &vertex, const std::string &fragment );


    bool SetUnifrom1f( std::string_view name, const float v );
    bool SetUnifrom2f( std::string_view name, const vec2f &v );
    bool SetUnifrom3f( std::string_view name, const vec3f &v );
    bool SetUnifrom4f( std::string_view name, const vec4f &v );
    bool SetUniformMatrix3f( std::string_view name, const mat3 &v );
    bool SetUniformMatrix4f( std::string_view name, const mat4 &v );

    void Bind();
    void Unbind();
};

inline unsigned int RenderProgram::GetProgram() const
{
    return shaderProgram;
}


#endif // RENDERPROGRAM_H
