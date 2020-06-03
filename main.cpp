#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "GAPI/GLmisc.h"
#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "thrid_party/stb_image.h"

#include "Transform.h"

#include "Game.h"

#include <iostream>
#include <memory>

namespace
{
const std::string vertexShader = R"(
#version 330 core

#define VERT_POSITION 0
#define VERT_TEXCOORD 1
#define VERT_COLOR 2

layout(location = VERT_POSITION) in vec3 inVertex;
layout(location = VERT_TEXCOORD) in vec2 inTexCoord;
layout(location = VERT_COLOR) in vec4 inColor;

uniform mat4 projView;
uniform mat4 model;

out vec2 TexCoord;
out vec4 Color;

void main()
{
    gl_Position = projView * model * vec4( inVertex, 1.0 );
    TexCoord = inTexCoord;
    Color = inColor;
})";

const std::string fragmentShader = R"(
#version 330 core

#define FRAG_OUTPUT_BUFFER0 0
uniform sampler2D texture0;

in vec2 TexCoord;
in vec4 Color;

layout( location = FRAG_OUTPUT_BUFFER0 ) out vec4 frag_color;

void main()
{
    frag_color = texture( texture0, TexCoord ) * ( Color / 255.0 );
}
)";
} // namespace

using SDLWindowPtr = std::unique_ptr<SDL_Window, void ( * )( SDL_Window * )>;
using SDLGLContextPtr = std::unique_ptr<void, void ( * )( SDL_GLContext )>;

int main(int argc, char **argv)
{
    Game game;
    game.ReadStats( "base//save.dat" );

    if( !game.texAtlas.LoadAtlas( "base//flappy.atlas" ) )
    {
        std::cout << "Missing texture atlas file\n";
        return 0;
    }

    SDL_SetMainReady();
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        std::cout << "Error init SDL.\n";
        return 0;
    }

    SDLWindowPtr handle( nullptr, SDL_DestroyWindow );
    SDLGLContextPtr glContext( nullptr, SDL_GL_DeleteContext );

    unsigned int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

    handle.reset( SDL_CreateWindow( "flappybird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, R_WINDOW_WIDTH,
                                    R_WINDOW_HEIGHT, flags ) );

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    glContext.reset( SDL_GL_CreateContext( handle.get() ) );

    SDL_GL_MakeCurrent( handle.get(), glContext.get() );

    SDL_GL_SetSwapInterval( 1 );

    if( !GL_InitGLEW() )
    {
        return 0;
    }

    glViewport( 0, 0, R_WINDOW_WIDTH, R_WINDOW_HEIGHT );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    GL_CheckError;

    if( !game.renderProg.CreateShaderProgramRaw( vertexShader, fragmentShader ) )
    {
        std::cout << "Error create shader program\n";
        return 0;
    }

    GL_CheckError;

    game.proj = glm::ortho( 0.0f, float( R_WINDOW_WIDTH ), 0.0f, float( R_WINDOW_HEIGHT ), -100.0f, 100.0f );
    game.view = mat4( 1.0f );
    game.projView = game.proj * game.view;

    game.renderProg.SetUniformMatrix4f( "projView", game.projView );

    int w;
    int h;
    int comp;
    unsigned char *image = stbi_load( "base//flappy.png", &w, &h, &comp, STBI_rgb_alpha );

    if( !image )
    {
        std::cout << "Error load texture\n";
        return 0;
    }

    game.texture.LoadFromMemory( image, w, h, false, false );

    stbi_image_free( image );

    game.LoadSpriteVertices( float( w ) );


    auto bgModel = game.MakeModel( "background" );
    bgModel.transform.SetScale(
        { float( R_WINDOW_WIDTH ) / bgModel.size.x, float( R_WINDOW_HEIGHT ) / bgModel.size.y, 0.0f } );


    std::array<PipeEnt, MAX_PIPES> pipes{ { { game.MakeModel( "pipe" ), game.MakeModel( "pipe" ) },
                                            { game.MakeModel( "pipe" ), game.MakeModel( "pipe" ) },
                                            { game.MakeModel( "pipe" ), game.MakeModel( "pipe" ) },
                                            { game.MakeModel( "pipe" ), game.MakeModel( "pipe" ) } } };

    PlayerEnt player{ game.MakeModel( "p_fly0" ) };
    player.model.transform.SetPosition( BIRD_INIT_POS );

    SinAnim calcBob( 6.0f, 2.0f );

    while( game.running )
    {
        game.thisTime = SDL_GetTicks();
        game.dt = game.thisTime - game.lastTime;

        SDL_Event e;

        while( SDL_PollEvent( &e ) )
        {
            if( e.type == SDL_QUIT || ( e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE ) )
            {
                game.running = false;
            }

            if( e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_SPACE )
            {
                game.GameNew( player, pipes );
            }

            if( e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT )
            {
                game.UpdatePlayerJump( player.bob, game.dt );
            }
        }

        if( game.state == GameState::MENU )
        {
            auto pos = player.model.transform.GetPosition();
            player.model.transform.SetPosition( { pos.x, pos.y + calcBob.Update( game.dt ), pos.z } );
        }

        if( game.state == GameState::PLAY )
        {
            game.UpdatePlayerMoving( player.bob, player.model, game.dt );
            game.PipeMoving( pipes, game.dt );
            game.PipeSpawning( pipes, game.rnd.Rand() );
        }

        game.UpdateAnimation( player.anim, player.model, game.dt );

        if( game.CheckCollisionWithPipes( player.model, pipes[0] ) || game.CheckCollisionWithGround( player.model ) )
        {
            game.GameLose( player );
        }

        game.CheckNewScore( player.model, pipes[0] );

        GL_Clear( true, true, false, 255, 0.0f, 0.0f, 0.0f, 1.0f );

        game.renderProg.Bind();

        game.vao.Bind();

        game.texture.Bind( 0 );

        game.renderProg.SetUniformMatrix4f( "model", bgModel.transform.GetMatrix() );
        auto spriteInfo = game.GetSprire( bgModel.hModel );
        glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr, spriteInfo.baseVertex );

        for( auto &&pipeEnt : pipes )
        {
            if( pipeEnt.disappeared || pipeEnt.bot.transform.GetPosition().x >= float( R_WINDOW_WIDTH ) )
            {
                continue;
            }

            auto spriteInfo = game.GetSprire( pipeEnt.bot.hModel );
            game.renderProg.SetUniformMatrix4f( "model", pipeEnt.bot.transform.GetMatrix() );
            glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr,
                                      spriteInfo.baseVertex );

            game.renderProg.SetUniformMatrix4f( "model", pipeEnt.top.transform.GetMatrix() );
            glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr,
                                      spriteInfo.baseVertex );
        }

        spriteInfo = game.GetSprire( player.model.hModel );
        game.renderProg.SetUniformMatrix4f( "model", player.model.transform.GetMatrix() );
        glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr, spriteInfo.baseVertex );

        game.DrawUI();

        game.vao.Unbind();
        game.renderProg.Unbind();

        SDL_GL_SwapWindow( handle.get() );

        GL_CheckError;

        game.lastTime = game.thisTime;
    }

    game.WriteStats( "base//save.dat" );

    SDL_Quit();
    return 0;
}
