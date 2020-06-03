#ifndef GAPI_H
#define GAPI_H

bool GL_InitGLEW();

void GL_Clear( bool color, bool depth, bool stencil, unsigned char stencilValue, float r, float g, float b, float a );
void GL_VertexAttrib( unsigned int index, int size, unsigned int type, int stride, unsigned int offset );

void GL_CheckError_( const char *file = "<unused>", int line = 0 );
#define GL_CheckError GL_CheckError_( ( __FILE__ ), ( __LINE__ ) )

#endif // GAPI_H
