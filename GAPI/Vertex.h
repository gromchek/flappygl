#ifndef VERTEX_H
#define VERTEX_H

#include "../Misc.h"

struct Vertex final
{
    vec3f position;
    vec2f texcoord;
    unsigned char color[4];
};

namespace VertexAttrib
{
const unsigned int POSITION_LOCATION = 0;
const unsigned int TEX_COORD_LOCATION = 1;
const unsigned int COLOR_LOCATION = 2;

const unsigned int OFFSET_POSITION = offsetof( Vertex, position );
const unsigned int OFFSET_TEX_COORD = offsetof( Vertex, texcoord );
const unsigned int OFFSET_COLOR = offsetof( Vertex, color );
} // namespace VertexAttrib

#endif // VERTEX_H
