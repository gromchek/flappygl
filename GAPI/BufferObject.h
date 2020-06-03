#ifndef BUFFEROBJECT_H
#define BUFFEROBJECT_H


enum MapBufferAccess : unsigned int
{
    ReadOnly = 0x88B8,
    WriteOnly = 0x88B9
};

enum class BufferUsage : unsigned int
{
    Vertex = 0x8892,
    Index = 0x8893,
    Uniform = 0x8A11
};


class Buffer
{
private:
    unsigned int selfTarget;
    unsigned int api;
    int use;
    unsigned int access;

public:
    Buffer();
    ~Buffer();

    unsigned int GetBuffer() const;
    void Update( unsigned int offset, unsigned int size, const void *date );
    void Init( BufferUsage type, bool readOnly = true );
    void Data( const void *data, unsigned int size );
    void *Map( unsigned int mode );
    void Unmap();

    void Bind();
    void Unbind();
};

inline Buffer::Buffer() : selfTarget( 0 ), api( 0 ), use( 0 ), access( 0 )
{
}

inline unsigned int Buffer::GetBuffer() const
{
    return api;
}


class VertexArrayObject
{
private:
    unsigned int vao;

public:
    VertexArrayObject();
    ~VertexArrayObject();

    void Bind();
    void Unbind();
};

inline VertexArrayObject::VertexArrayObject() : vao( 0 )
{
}

#endif // BUFFEROBJECT_H
