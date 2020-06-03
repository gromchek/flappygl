#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>

mat4 Transform::GetMatrix()
{
    if( updateMatrix )
    {
        matrix = glm::translate( mat4( 1.0f ), position );

        if( rot.w != 0.0f )
        {
            matrix = glm::translate( matrix, origin );
            matrix = glm::rotate( matrix, glm::radians( rot.w ), vec3f( rot ) );
            matrix = glm::translate( matrix, -origin );
        }

        if( scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f )
        {
            matrix = glm::translate( matrix, origin );
            matrix = glm::scale( matrix, scale );
            matrix = glm::translate( matrix, -origin );
        }

        updateMatrix = false;
    }

    return matrix;
}
