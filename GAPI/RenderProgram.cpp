#include "RenderProgram.h"

#include <cassert>

#include "GLmisc.h"
#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

namespace
{

std::pair<std::size_t, int> glToIndexType[] = { { GL_FLOAT, 1 },	  { GL_FLOAT_VEC2, 2 },	 { GL_FLOAT_VEC3, 3 },
                                                { GL_FLOAT_VEC4, 4 }, { GL_FLOAT_MAT3, 12 }, { GL_FLOAT_MAT4, 16 } };
} // namespace

unsigned int RenderProgram::loadGLSLShader( const std::string &content, GLuint type )
{
    unsigned int shader_obj = glCreateShader( type );

    if( glIsShader( shader_obj ) )
    {
        const char *shader_text = content.c_str();
        int len = int( content.length() );

        glShaderSource( shader_obj, 1, static_cast<const char **>( &shader_text ), &len );

        glCompileShader( shader_obj );

        int maxLength = 0;
        glGetShaderiv( shader_obj, GL_INFO_LOG_LENGTH, &maxLength );

        if( maxLength > 1 )
        {
            char log[1024];
            glGetShaderInfoLog( shader_obj, sizeof( log ), nullptr, &log[0] );
            printf( "RenderProgram::LoadGLSLShader: While compiling shader '%s' type (Log: '%s')\n", content.data(),
                    log );
        }

        GL_CheckError;

        int success = GL_TRUE;

        glGetShaderiv( shader_obj, GL_COMPILE_STATUS, &success );
        if( success == GL_FALSE )
        {
            glDeleteShader( shader_obj );
            return INVALID_PROGID;
        }

        GL_CheckError;

        return shader_obj;
    }
    return INVALID_PROGID;
}


bool RenderProgram::CreateShaderProgramRaw( const std::string &vertex, const std::string &fragment )
{
    auto vert_shader = loadGLSLShader( vertex, GL_VERTEX_SHADER );
    if( vert_shader == INVALID_PROGID )
    {
        return false;
    }

    GL_CheckError;

    auto frag_shader = loadGLSLShader( fragment, GL_FRAGMENT_SHADER );
    if( frag_shader == INVALID_PROGID )
    {
        return false;
    }

    GL_CheckError;

    shaderProgram = glCreateProgram();

    GL_CheckError;

    if( !shaderProgram )
    {
        printf( "RenderProgram::CreateShaderProgram: Error create program:\nVertex '%s'\nFragment '%s'\n",
                vertex.data(), fragment.data() );
        return false;
    }

    glAttachShader( shaderProgram, vert_shader );
    glAttachShader( shaderProgram, frag_shader );

    GL_CheckError;

    glLinkProgram( shaderProgram );

    if( !link() )
    {
        glDetachShader( shaderProgram, vert_shader );
        glDetachShader( shaderProgram, frag_shader );

        glDeleteShader( vert_shader );
        glDeleteShader( frag_shader );

        glDeleteProgram( shaderProgram );
        return false;
    }

    loadUniforms();

    glDetachShader( shaderProgram, vert_shader );
    glDetachShader( shaderProgram, frag_shader );

    glDeleteShader( vert_shader );
    glDeleteShader( frag_shader );

    GL_CheckError;

    validate();

    return true;
}

bool RenderProgram::SetUnifrom1f( std::string_view name, const float v )
{
    return setUniformf( name, &v );
}
bool RenderProgram::SetUnifrom2f( std::string_view name, const vec2f &v )
{
    return setUniformf( name, glm::value_ptr( v ) );
}
bool RenderProgram::SetUnifrom3f( std::string_view name, const vec3f &v )
{
    return setUniformf( name, glm::value_ptr( v ) );
}
bool RenderProgram::SetUnifrom4f( std::string_view name, const vec4f &v )
{
    return setUniformf( name, glm::value_ptr( v ) );
}
bool RenderProgram::SetUniformMatrix3f( std::string_view name, const mat3 &v )
{
    return setUniformf( name, glm::value_ptr( v ) );
}
bool RenderProgram::SetUniformMatrix4f( std::string_view name, const mat4 &v )
{
    return setUniformf( name, glm::value_ptr( v ) );
}

bool RenderProgram::link()
{
    int success = GL_TRUE;

    glGetProgramiv( shaderProgram, GL_LINK_STATUS, &success );
    if( success == GL_FALSE )
    {
        char log[1024];
        glGetProgramInfoLog( shaderProgram, sizeof( log ), nullptr, &log[0] );
        printf( "RenderProgram::LinkStatus: Error linking shader program: '%s'\n", log );

        return false;
    }
    return true;
}

bool RenderProgram::validate()
{
    Bind();
    int success = GL_TRUE;

    glGetProgramiv( shaderProgram, GL_VALIDATE_STATUS, &success );
    Unbind();
    if( success == GL_FALSE )
    {
        char log[1024];
        glGetProgramInfoLog( shaderProgram, sizeof( log ), nullptr, &log[0] );
        printf( "RenderProgram::ValidateStatus: Error validate shader program: '%s'\n", log );

        return false;
    }
    return true;
}

void RenderProgram::loadUniforms()
{
    Bind();

    int uniformCount = 0;
    glGetProgramiv( shaderProgram, GL_ACTIVE_UNIFORMS, &uniformCount );

    if( uniformCount > MAX_UNIFORMS )
    {
        uniformCount = MAX_UNIFORMS;
        printf( "RenderProgram::LoadUniforms: Error 'uniformCount' value." );
    }

    for( int i = 0; i < uniformCount; i++ )
    {
        int namelen;
        int size = 0;
        unsigned int type;
        char name[32];
        glGetActiveUniform( shaderProgram, i, sizeof( name ), &namelen, &size, &type, name );

        if( size )
        {
            int l = glGetUniformLocation( shaderProgram, name );
            Uniform &u = UniformInfo[i];

            u.Name = name;
            u.Location = l;
            u.Size = size;

            for( auto ii = 0u; ii < std::size( glToIndexType ); ii++ )
            {
                if( glToIndexType[ii].first == type )
                {
                    u.Type = glToIndexType[ii].second;
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }
    for( int i = 0; i < 8; i++ )
    {
        char texv[12];
        sprintf( texv, "texture%d", i );
        texLoc[i] = glGetUniformLocation( shaderProgram, texv );
        if( texLoc[i] < 0 )
        {
            break;
        }
        glUniform1i( texLoc[i], i );
    }
    Unbind();
}

bool RenderProgram::setUniformf( std::string_view name, const float *v )
{
    for( unsigned int i = 0; i < MAX_UNIFORMS; i++ )
    {
        if( UniformInfo[i].Name == name )
        {
            Bind();
            switch( UniformInfo[i].Type )
            {
            case 1:
                glUniform1fv( UniformInfo[i].Location, 1, v );
                break;
            case 2:
                glUniform2fv( UniformInfo[i].Location, 1, v );
                break;
            case 3:
                glUniform3fv( UniformInfo[i].Location, 1, v );
                break;
            case 4:
                glUniform4fv( UniformInfo[i].Location, 1, v );
                break;
            case 12:
                glUniformMatrix3fv( UniformInfo[i].Location, 1, GL_FALSE, v );
                break;
            case 16:
                glUniformMatrix4fv( UniformInfo[i].Location, 1, GL_FALSE, v );
                break;
            default:
                assert( 0 );
                break;
            }
            return true;
        }
    }

    return false;
}


RenderProgram::RenderProgram() : shaderProgram( 0 ), vertexShader( 0 ), fragmentShader( 0 )
{
}

RenderProgram::~RenderProgram()
{
    Unbind();
    if( shaderProgram )
    {
        glDeleteProgram( shaderProgram );
    }
}

void RenderProgram::Bind()
{
    glUseProgram( shaderProgram );
}

void RenderProgram::Unbind()
{
    glUseProgram( 0 );
}
