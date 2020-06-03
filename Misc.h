#ifndef MISC_H
#define MISC_H

#include <glm/glm.hpp>

using vec4f = glm::vec4;

using vec2f = glm::vec2;
using vec2i = glm::ivec2;
using vec2u = glm::uvec2;

using vec3f = glm::vec3;
using vec3i = glm::ivec3;
using vec3u = glm::uvec3;

using FloatRect = glm::vec4;
using IntRect = glm::ivec4;

using mat3 = glm::mat3;
using mat4 = glm::mat4;


constexpr int R_WINDOW_WIDTH = 1024;
constexpr int R_WINDOW_HEIGHT = 768;

constexpr unsigned int TUBE_VERT_GAP_SIZE = 200;
constexpr unsigned int TUBE_HORZ_GAP_SIZE = ( R_WINDOW_WIDTH / 5 ) * 2;

constexpr unsigned int TUBE_WIDTH = R_WINDOW_WIDTH / 10;
constexpr unsigned int TUBE_MIN_HEIGHT = 130;
constexpr unsigned int TUBE_MAX_HEIGHT = R_WINDOW_HEIGHT - TUBE_MIN_HEIGHT - TUBE_VERT_GAP_SIZE;

constexpr unsigned int MAX_PIPES = 4;

#endif // MISC_H
