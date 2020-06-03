#include "BufferObject.h"

#include <GL/glew.h>

Buffer::~Buffer()
{
    glBindBuffer( selfTarget, 0 );
    if( api )
    {
        glDeleteBuffers( 1, &api );
    }
}

void Buffer::Data( const void *data, unsigned int size )
{
    glBindBuffer( selfTarget, api );
    glBufferData( selfTarget, size, data, access );
}

void *Buffer::Map( unsigned int mode )
{
    glBindBuffer( selfTarget, api );
    void *v = glMapBuffer( selfTarget, mode );
    return v;
}

void Buffer::Unmap()
{
    glUnmapBuffer( selfTarget );
}

void Buffer::Init( BufferUsage type, bool readOnly )
{
    access = GL_DYNAMIC_DRAW;
    if( readOnly )
    {
        access = GL_STATIC_DRAW;
    }

    selfTarget = unsigned( type );
    glGenBuffers( 1, &api );
}

void Buffer::Update( unsigned int offset, unsigned int size, const void *data )
{
    glBindBuffer( selfTarget, api );
    glBufferSubData( selfTarget, offset, size, data );
}

void Buffer::Bind()
{
    glBindBuffer( selfTarget, api );
}

void Buffer::Unbind()
{
    glBindBuffer( selfTarget, 0 );
}

VertexArrayObject::~VertexArrayObject()
{
    glBindVertexArray( 0 );
    if( vao )
    {
        glDeleteVertexArrays( 1, &vao );
    }
}

void VertexArrayObject::Bind()
{
    if( !vao )
    {
        glGenVertexArrays( 1, &vao );
        glBindVertexArray( vao );
    }
    else
    {
        glBindVertexArray( vao );
    }
}
void VertexArrayObject::Unbind()
{
    glBindVertexArray( 0 );
}
