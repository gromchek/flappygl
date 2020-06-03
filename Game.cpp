#include "Game.h"

#include "GAPI/Vertex.h"
#include "GAPI/GLmisc.h"
#include <GL/glew.h>

#include <fstream>
#include <algorithm>

#include <ctime>

namespace
{
bool CheckRectCollision( const vec4f &a, const vec4f &b )
{
    float r1MinX = std::min( a.x, ( a.x + a.z ) );
    float r1MaxX = std::max( a.x, ( a.x + a.z ) );
    float r1MinY = std::min( a.y, ( a.y + a.w ) );
    float r1MaxY = std::max( a.y, ( a.y + a.w ) );

    float r2MinX = std::min( b.x, ( b.x + b.z ) );
    float r2MaxX = std::max( b.x, ( b.x + b.z ) );
    float r2MinY = std::min( b.y, ( b.y + b.w ) );
    float r2MaxY = std::max( b.y, ( b.y + b.w ) );

    float interLeft = std::max( r1MinX, r2MinX );
    float interTop = std::max( r1MinY, r2MinY );
    float interRight = std::min( r1MaxX, r2MaxX );
    float interBottom = std::min( r1MaxY, r2MaxY );

    if( ( interLeft < interRight ) && ( interTop < interBottom ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

const int primesNumbers[16] = { 4493, 5569, 2399, 1861, 2113, 607,	7253, 7451,
                                3863, 2999, 1597, 1567, 1223, 2381, 7247, 307 };

const int ANIM_MAX_FRAME_TIME = 100;
} // namespace


void makeFaceVerts( const FloatRect &rect, Vertex *vert, const float textureSize )
{
    if( !vert )
    {
        return;
    }

    const float x = rect.x / textureSize;
    const float y = rect.y / textureSize;
    const float w = rect.z / textureSize;
    const float h = rect.w / textureSize;

    // top-left
    /*vert[0] = { { 0.0f, 0.0f, 0.0f }, { x, y }, { 255, 255, 255, 255 } };
    vert[1] = { { rect.z, 0.0f, 0.0f }, { x + w, y }, { 255, 255, 255, 255 } };
    vert[2] = { { rect.z, rect.w, 0.0f }, { x + w, y + h }, { 255, 255, 255, 255 } };
    vert[3] = { { 0.0f, rect.w, 0.0f }, { x, y + h }, { 255, 255, 255, 255 } };*/

    // bot-left
    vert[0] = { { 0.0f, 0.0f, 0.0f }, { x, y + h }, { 255, 255, 255, 255 } };
    vert[1] = { { rect.z, 0.0f, 0.0f }, { x + w, y + h }, { 255, 255, 255, 255 } };
    vert[2] = { { rect.z, rect.w, 0.0f }, { x + w, y }, { 255, 255, 255, 255 } };
    vert[3] = { { 0.0f, rect.w, 0.0f }, { x, y }, { 255, 255, 255, 255 } };
}

Game::Game() :
    rnd( primesNumbers[( std::time( nullptr ) + 1 ) & std::size( primesNumbers )], int( TUBE_MIN_HEIGHT ),
         int( TUBE_MAX_HEIGHT ) )
{
}

void Game::ReadStats( const std::string &fileName )
{
    std::ifstream file( fileName, std::ifstream::binary | std::ifstream::ate );

    if( !file )
    {
        return;
    }

    const auto fileSize = file.tellg();

    if( fileSize != sizeof( int ) )
    {
        return;
    }

    file.seekg( 0, std::ifstream::beg );

    file.read( (char *)( &bestResult ), sizeof( int ) );
}

void Game::WriteStats( const std::string &fileName )
{
    std::ofstream file( fileName, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary );
    file.write( (char *)( &bestResult ), sizeof( int ) );
}

void Game::LoadSpriteVertices( const float textureSize )
{
    const auto &atlasList = texAtlas.GetAtlasList();
    if( atlasList.empty() )
    {
        return;
    }

    const std::size_t GL_QUAD_VERT_COUNT = 6;
    const std::size_t QUAD_VERT_COUNT = 4;

    std::vector<Vertex> vertexBuff;
    vertexBuff.resize( QUAD_VERT_COUNT * atlasList.size() );

    spriteData.reserve( atlasList.size() );

    int vertNum = 0;

    for( auto &&info : atlasList )
    {
        Vertex *vert = &vertexBuff[std::size_t( vertNum )];
        makeFaceVerts( info.rect, vert, textureSize );

        spriteData.push_back( { info.name, { info.rect.z, info.rect.w }, vertNum, GL_QUAD_VERT_COUNT } );
        vertNum += QUAD_VERT_COUNT;
    }

    auto defaultIter =
        std::find_if( atlasList.begin(), atlasList.end(), []( const auto atl ) { return atl.name == "_default"; } );

    Vertex vert[4];
    makeFaceVerts( defaultIter->rect, vert, textureSize );

    vert[0].color[0] = vert[1].color[0] = 253;
    vert[0].color[1] = vert[1].color[1] = 228;
    vert[0].color[2] = vert[1].color[2] = 133;
    vert[0].color[3] = vert[1].color[3] = 255;

    vert[2].color[0] = vert[3].color[0] = 120;
    vert[2].color[1] = vert[3].color[1] = 215;
    vert[2].color[2] = vert[3].color[2] = 255;
    vert[2].color[3] = vert[3].color[3] = 255;

    for( auto &&v : vert )
    {
        vertexBuff.push_back( v );
    }

    spriteData.push_back( { "background", { defaultIter->rect.z, defaultIter->rect.w }, vertNum, GL_QUAD_VERT_COUNT } );
    vertNum += QUAD_VERT_COUNT;

    vao.Bind();

    vertexBuffer.Init( BufferUsage::Vertex, true );
    vertexBuffer.Data( vertexBuff.data(), vertexBuff.size() * sizeof( Vertex ) );

    const unsigned short index[] = { 0, 1, 2, 0, 2, 3 };
    indexBuffer.Init( BufferUsage::Index, true );
    indexBuffer.Data( index, sizeof( index ) );

    GL_VertexAttrib( VertexAttrib::POSITION_LOCATION, 3, GL_FLOAT, sizeof( Vertex ), VertexAttrib::OFFSET_POSITION );
    GL_VertexAttrib( VertexAttrib::TEX_COORD_LOCATION, 2, GL_FLOAT, sizeof( Vertex ), VertexAttrib::OFFSET_TEX_COORD );
    GL_VertexAttrib( VertexAttrib::COLOR_LOCATION, 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), VertexAttrib::OFFSET_COLOR );

    vao.Unbind();
}

Model Game::MakeModel( std::string_view name )
{
    auto it = std::find_if( spriteData.begin(), spriteData.end(), [&name]( const auto m ) { return m.name == name; } );

    if( it != spriteData.end() )
    {
        return { Transform( mat4( 1.0f ) ),
                 it->baseSize,
                 std::size_t( it - spriteData.begin() ),
                 { 0.0f, 0.0f, it->baseSize.x, it->baseSize.y } };
    }

    auto defaultIt =
        std::find_if( spriteData.begin(), spriteData.end(), []( const auto m ) { return m.name == "_default"; } );

    return { Transform( mat4( 1.0f ) ),
             defaultIt->baseSize,
             std::size_t( defaultIt - spriteData.begin() ),
             { 0.0f, 0.0f, it->baseSize.x, it->baseSize.y } };
}

std::size_t Game::GetSpriteIndex( std::string_view name ) const
{
    auto it = std::find_if( spriteData.begin(), spriteData.end(), [&name]( const auto m ) { return m.name == name; } );

    if( it != spriteData.end() )
    {
        return std::size_t( it - spriteData.begin() );
    }

    auto defaultIt =
        std::find_if( spriteData.begin(), spriteData.end(), []( const auto m ) { return m.name == "_default"; } );

    return std::size_t( defaultIt - spriteData.begin() );
}

Sprite Game::GetSprire( const std::size_t index ) const
{
    if( spriteData.size() <= index )
    {
        auto defaultIt =
            std::find_if( spriteData.begin(), spriteData.end(), []( const auto m ) { return m.name == "_default"; } );

        return *defaultIt;
    }

    return spriteData[index];
}

void Game::PipeMoving( std::array<PipeEnt, MAX_PIPES> &pipes, const unsigned int delta )
{
    const float pos = delta * 0.22f + ( scores * 0.1f );

    for( auto &pipe : pipes )
    {
        auto botPos = pipe.bot.transform.GetPosition();
        pipe.bot.transform.SetPosition( { botPos.x + -pos, botPos.y, botPos.z } );
        pipe.bot.bbox.x = pipe.bot.transform.GetPosition().x;
        pipe.bot.bbox.y = pipe.bot.transform.GetPosition().y;

        auto topPos = pipe.top.transform.GetPosition();
        pipe.top.transform.SetPosition( { topPos.x + -pos, topPos.y, topPos.z } );
        pipe.top.bbox.x = pipe.top.transform.GetPosition().x;
        pipe.top.bbox.y = pipe.top.transform.GetPosition().y;


        const float posX = pipe.bot.transform.GetPosition().x + pipe.bot.size.x * pipe.bot.transform.GetScale().x;
        if( posX < 0.0f )
        {
            pipe.disappeared = true;
        }
    }
}

void Game::PipeSpawning( std::array<PipeEnt, MAX_PIPES> &pipes, const int vertSize )
{
    std::sort( pipes.begin(), pipes.end(), []( const auto &l, const auto &r ) {
        return l.bot.transform.GetPosition().x < r.bot.transform.GetPosition().x;
    } );

    for( auto &pipe : pipes )
    {
        if( !pipe.disappeared )
        {
            continue;
        }

        const float lastPipePos = pipes[MAX_PIPES - 1].bot.transform.GetPosition().x;

        const float botPos = vertSize - pipe.bot.size.y * pipe.bot.transform.GetScale().y;
        pipe.bot.transform.SetPosition( { lastPipePos + TUBE_HORZ_GAP_SIZE, botPos, 0.0f } );

        const float topPos = vertSize + float( TUBE_VERT_GAP_SIZE );
        pipe.top.transform.SetPosition( { lastPipePos + TUBE_HORZ_GAP_SIZE, topPos, 0.0f } );

        pipe.disappeared = false;
        pipe.collisionProcessed = false;
        pipe.scoreGained = false;

        break;
    }
}

void Game::CheckNewScore( const Model &model, PipeEnt &pipe )
{
    const float pos = model.transform.GetPosition().x + model.size.x / 2.0f;

    if( pos > pipe.bot.transform.GetPosition().x && !pipe.scoreGained )
    {
        pipe.scoreGained = true;
        scores++;
    }
}

bool Game::CheckCollisionWithPipes( const Model &model, PipeEnt &pipe )
{
    vec4f bbox = model.bbox;

    bbox.z *= 0.9f;
    bbox.w *= 0.9f;

    if( pipe.collisionProcessed || pipe.disappeared )
    {
        return false;
    }

    pipe.collisionProcessed =
        ( CheckRectCollision( bbox, pipe.bot.bbox ) || CheckRectCollision( bbox, pipe.top.bbox ) );

    return pipe.collisionProcessed;
}

bool Game::CheckCollisionWithGround( const Model &model )
{
    const float pos = model.transform.GetPosition().y + model.size.y * 0.5f;

    if( pos < 0.0f )
    {
        return true;
    }

    return false;
}

void Game::UpdateAnimation( Animation &anim, Model &model, const unsigned int delta )
{
    anim.frameTime += delta;

    if( anim.frameTime > ANIM_MAX_FRAME_TIME )
    {
        anim.frameTime = 0;
        anim.currFrame += 1;
        anim.currFrame %= 2;
    }

    std::string animName( "p_fly " );

    if( anim.state == AnimState::DEAD )
    {
        animName = "p_dead ";
    }

    switch( anim.currFrame )
    {
    case 0:
        animName.back() = '0';
        model.hModel = GetSpriteIndex( animName );
        break;
    case 1:
        animName.back() = '1';
        model.hModel = GetSpriteIndex( animName );
        break;
    default:
        break;
    }
}

void Game::UpdatePlayerMoving( Bobbing &bob, Model &model, const unsigned int delta )
{
    bob.v -= 0.03f * delta;
    bob.angle += 0.07f * delta;
    bob.v = std::min( 10.0f, std::max( -10.0f, bob.v ) );
    bob.angle = std::min( 90.0f, std::max( 0.0f, bob.angle ) );

    const auto &lastBirdPos = model.transform.GetPosition();
    model.transform.SetPosition( { lastBirdPos.x, lastBirdPos.y + bob.v, lastBirdPos.z } );
    model.transform.SetRotate( -bob.angle, { 0.0f, 0.0f, 1.0f } );

    model.bbox.x = model.transform.GetPosition().x;
    model.bbox.y = model.transform.GetPosition().y;
}

void Game::UpdatePlayerJump( Bobbing &bob, const unsigned int delta )
{
    bob.v += 1.1f * delta;
    bob.angle -= 15.0f * delta;
}

void Game::GameNew( PlayerEnt &player, std::array<PipeEnt, MAX_PIPES> &pipes )
{
    if( state == GameState::PLAY )
    {
        return;
    }

    pipes = { { { MakeModel( "pipe" ), MakeModel( "pipe" ) },
                { MakeModel( "pipe" ), MakeModel( "pipe" ) },
                { MakeModel( "pipe" ), MakeModel( "pipe" ) },
                { MakeModel( "pipe" ), MakeModel( "pipe" ) } } };

    for( auto i = 0u; i < pipes.size(); i++ )
    {
        PipeSpawning( pipes, rnd.Rand() );
    }


    player = { MakeModel( "p_fly0" ) };
    player.model.transform.SetPosition( BIRD_INIT_POS );

    state = GameState::PLAY;
    scores = 0;
}

void Game::GameLose( PlayerEnt &player )
{
    state = GameState::LOSE;
    player.anim.state = AnimState::DEAD;

    bestResult = scores > bestResult ? scores : bestResult;
}

void Game::DrawUI()
{
    drawMenu();

    if( state == GameState::PLAY )
    {
        drawScore( scores, { R_WINDOW_WIDTH * 0.5f, R_WINDOW_HEIGHT * 0.875f } );
    }

    drawEnd();
}

void Game::drawMenu()
{
    if( state != GameState::MENU )
    {
        return;
    }

    auto gameTitle = MakeModel( "gametitle" );
    gameTitle.transform.SetPosition(
        { R_WINDOW_WIDTH * 0.5f - gameTitle.size.x / 2.0f, R_WINDOW_HEIGHT * 0.875f, 0.0f } );

    auto spriteInfo = GetSprire( gameTitle.hModel );
    renderProg.SetUniformMatrix4f( "model", gameTitle.transform.GetMatrix() );
    glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr, spriteInfo.baseVertex );


    auto restart = MakeModel( "gamestart" );
    restart.transform.SetPosition( { R_WINDOW_WIDTH * 0.5f - restart.size.x / 2.0f, R_WINDOW_HEIGHT * 0.66f, 0.0f } );

    spriteInfo = GetSprire( restart.hModel );
    renderProg.SetUniformMatrix4f( "model", restart.transform.GetMatrix() );
    glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr, spriteInfo.baseVertex );
}

void Game::drawScore( const int value, const vec2f &pos )
{
    std::string s( std::to_string( value ) );

    Transform digitTrans;
    digitTrans.SetPosition( { pos.x, pos.y, 0.0f } );

    std::string dig( "dig " );

    for( auto i = 0u; i < s.length(); i++ )
    {
        dig.back() = s[i];

        auto spriteInfo = GetSprire( GetSpriteIndex( dig ) );

        renderProg.SetUniformMatrix4f( "model", digitTrans.GetMatrix() );
        glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr, spriteInfo.baseVertex );

        const auto &pos = digitTrans.GetPosition();
        digitTrans.SetPosition( { pos.x + spriteInfo.baseSize.x, pos.y, pos.z } );
    }
}

void Game::drawEnd()
{
    if( state != GameState::LOSE )
    {
        return;
    }

    auto spriteTexture = MakeModel( "gameover" );
    spriteTexture.transform.SetPosition(
        { R_WINDOW_WIDTH * 0.5f - spriteTexture.size.x / 2.0f, R_WINDOW_HEIGHT * 0.75f, 0.0f } );

    auto spriteInfo = GetSprire( spriteTexture.hModel );
    renderProg.SetUniformMatrix4f( "model", spriteTexture.transform.GetMatrix() );
    glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr, spriteInfo.baseVertex );


    auto scoreSprite = MakeModel( "score" );
    scoreSprite.transform.SetPosition(
        { R_WINDOW_WIDTH * 0.5f - scoreSprite.size.x * 0.75f + 32.0f, R_WINDOW_HEIGHT * 0.5f, 0.0f } );

    spriteInfo = GetSprire( scoreSprite.hModel );
    renderProg.SetUniformMatrix4f( "model", scoreSprite.transform.GetMatrix() );
    glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr, spriteInfo.baseVertex );

    drawScore( scores,
               { scoreSprite.transform.GetPosition().x + scoreSprite.size.x, scoreSprite.transform.GetPosition().y } );

    auto bestSprite = MakeModel( "best" );
    bestSprite.transform.SetPosition(
        { R_WINDOW_WIDTH * 0.5f - bestSprite.size.x * 0.75f + 32.0f, R_WINDOW_HEIGHT * 0.4f, 0.0f } );

    spriteInfo = GetSprire( bestSprite.hModel );
    renderProg.SetUniformMatrix4f( "model", bestSprite.transform.GetMatrix() );
    glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr, spriteInfo.baseVertex );

    drawScore( bestResult,
               { bestSprite.transform.GetPosition().x + bestSprite.size.x, bestSprite.transform.GetPosition().y } );


    auto restart = MakeModel( "gamestart" );
    restart.transform.SetPosition( { R_WINDOW_WIDTH * 0.5f - restart.size.x / 2.0f, R_WINDOW_HEIGHT * 0.25f, 0.0f } );

    spriteInfo = GetSprire( restart.hModel );
    renderProg.SetUniformMatrix4f( "model", restart.transform.GetMatrix() );
    glDrawElementsBaseVertex( GL_TRIANGLES, spriteInfo.count, GL_UNSIGNED_SHORT, nullptr, spriteInfo.baseVertex );
}
