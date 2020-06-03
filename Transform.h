#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Misc.h"

class Transform final
{
private:
    mat4 matrix = mat4( 1.0f );

    vec3f position = vec3f( 0.0f );
    vec4f rot = vec4f( 0.0f );
    vec3f scale = vec3f( 1.0f );

    vec3f origin = vec3f( 0.0f );

    bool updateMatrix = false;

public:
    Transform() = default;
    explicit Transform( const mat4 &modelMatrix );
    ~Transform() = default;

    void SetPosition( const vec3f &pos );
    const vec3f &GetPosition() const;

    void SetScale( const vec3f &scaling );
    const vec3f GetScale() const;

    void SetOrigin( const vec3f &orig );
    const vec3f &GetOrigin() const;

    void SetRotate( const float a, const vec3f &o );
    const vec4f &GetRotate() const;

    mat4 GetMatrix();
};

inline Transform::Transform( const mat4 &modelMatrix ) : matrix( modelMatrix )
{
}

inline void Transform::SetPosition( const vec3f &pos )
{
    position = pos;
    updateMatrix = true;
}

inline const vec3f &Transform::GetPosition() const
{
    return position;
}

inline void Transform::SetScale( const vec3f &scaling )
{
    scale = scaling;
    updateMatrix = true;
}

inline const vec3f Transform::GetScale() const
{
    return scale;
}

inline void Transform::SetOrigin( const vec3f &orig )
{
    origin = orig;
}

inline const vec3f &Transform::GetOrigin() const
{
    return origin;
}

inline void Transform::SetRotate( const float a, const vec3f &o )
{
    float ang = static_cast<float>( fmodf( a, 360.0f ) );

    if( ang < 0 )
    {
        ang += 360.0f;
    }

    rot = vec4f( o, ang );
    updateMatrix = true;
}

inline const vec4f &Transform::GetRotate() const
{
    return rot;
}

#endif // TRANSFORM_H
