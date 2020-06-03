#ifndef GAME_H
#define GAME_H

#include "Misc.h"
#include "Transform.h"
#include <vector>
#include <string>
#include <array>
#include <random>

#include "TextureAtlas.h"

#include "GAPI/BufferObject.h"
#include "GAPI/RenderProgram.h"
#include "GAPI/Texture.h"

const vec3f BIRD_INIT_POS = { 128.0f, R_WINDOW_HEIGHT - 256.0f, 1.0f };

enum class AnimState
{
    ALIVE,
    DEAD
};

enum class GameState
{
    MENU,
    PLAY,
    LOSE
};

struct Sprite
{
    std::string name;

    vec2f baseSize;
    int baseVertex;
    int count;
};

struct Model
{
    Transform transform;
    vec2f size;
    std::size_t hModel = 0;
    vec4f bbox;
};

class RandomGen final
{
private:
    std::mt19937 mt;
    std::uniform_int_distribution<int> dis;

public:
    RandomGen( int seed, int min, int max ) : mt( seed ), dis( min, max )
    {
    }

    int Rand()
    {
        return dis( mt );
    }
};

struct SinAnim
{
    float value = 0.0f;
    float dtMod = 1.0f;
    float valMod = 4.0f;

    const float PI2 = 3.1415926f * 2.0f;

    SinAnim( const float dtScale, const float valScale ) : dtMod( dtScale ), valMod( valScale )
    {
    }

    float Update( const unsigned int dt )
    {
        float bob = std::sin( value ) * valMod;

        if( value > PI2 )
        {
            value = 0.0f;
        }

        value += dt * 0.001f * dtMod;

        return bob;
    }
};

struct Animation
{
    Animation() : state( AnimState::ALIVE ), frameTime( 0 ), currFrame( 0 )
    {
    }

    AnimState state = AnimState::ALIVE;

    int frameTime = 0;
    int currFrame = 0;
};

struct Bobbing
{
    Bobbing() : v( 0.0f ), angle( 0.0f )
    {
    }
    float v = 0.0f;
    float angle = 0.0f;
};

struct PlayerEnt
{
    PlayerEnt() = default;

    PlayerEnt( const Model &m ) : anim( {} ), bob( {} ), model( m )
    {
        model.transform.SetOrigin( { model.size.x / 2.0f, model.size.y / 2.0f, 0.0f } );

        model.bbox.z = model.size.x;
        model.bbox.w = model.size.y;
    }

    Animation anim;
    Bobbing bob;
    Model model;
};

struct PipeEnt
{
    PipeEnt() = default;

    PipeEnt( const Model &t, const Model &b ) : top( t ), bot( b )
    {
        const float yScale = 10.0f;

        top.transform.SetPosition( { float( R_WINDOW_WIDTH ), 0.0f, 0.0f } );
        bot.transform.SetPosition( { float( R_WINDOW_WIDTH ), 0.0f, 0.0f } );

        const float widthScale = float( TUBE_WIDTH ) / float( bot.size.x );

        top.transform.SetScale( { widthScale, yScale, 1.0f } );
        bot.transform.SetScale( { widthScale, yScale, 1.0f } );

        top.bbox.x = float( R_WINDOW_WIDTH );
        top.bbox.y = 0.0f;
        top.bbox.z = float( TUBE_WIDTH );
        top.bbox.w = float( top.size.y ) * yScale * 4.0f; // extra size

        bot.bbox.x = float( R_WINDOW_WIDTH );
        bot.bbox.y = 0.0f;
        bot.bbox.z = float( TUBE_WIDTH );
        bot.bbox.w = float( bot.size.y ) * yScale;

        disappeared = true;
        collisionProcessed = false;
        scoreGained = false;
    }

    Model top;
    Model bot;

    bool disappeared = true;
    bool collisionProcessed = false;
    bool scoreGained = false;
};


struct Game
{
    Game();

    unsigned int dt = 0;
    unsigned int thisTime = 0;
    unsigned int lastTime = 0;

    bool running = true;

    RandomGen rnd;

    GameState state = GameState::MENU;

    void ReadStats( const std::string &fileName );
    void WriteStats( const std::string &fileName );

    TextureAtlas texAtlas;

    void LoadSpriteVertices( const float textureSize );
    Model MakeModel( std::string_view name );

    std::size_t GetSpriteIndex( std::string_view name ) const;
    Sprite GetSprire( const std::size_t index ) const;

    void PipeMoving( std::array<PipeEnt, MAX_PIPES> &pipes, const unsigned int delta );
    void PipeSpawning( std::array<PipeEnt, MAX_PIPES> &pipes, const int vertSize );
    void CheckNewScore( const Model &model, PipeEnt &pipe );

    bool CheckCollisionWithPipes( const Model &model, PipeEnt &pipe );
    bool CheckCollisionWithGround( const Model &model );

    void UpdateAnimation( Animation &anim, Model &model, const unsigned int delta );
    void UpdatePlayerMoving( Bobbing &bob, Model &model, const unsigned int delta );
    void UpdatePlayerJump( Bobbing &bob, const unsigned int delta );

    void GameNew( PlayerEnt &player, std::array<PipeEnt, MAX_PIPES> &pipes );
    void GameLose( PlayerEnt &player );

    VertexArrayObject vao;
    Buffer vertexBuffer;
    Buffer indexBuffer;

    RenderProgram renderProg;
    Texture texture;

    mat4 proj;
    mat4 view;

    mat4 projView;

    void DrawUI();

private:
    std::vector<Sprite> spriteData;

    void drawMenu();
    void drawScore( const int value, const vec2f &pos = { 0.0f, 0.0f } );
    void drawEnd();

    int scores = 0;
    int bestResult = 0;
};

#endif // GAME_H
