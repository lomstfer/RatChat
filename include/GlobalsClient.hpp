#include "Globals.hpp"
#include <raylib.h>

std::ostream& operator<<(std::ostream& stream, const rl::Vector2& vec2)
{
    stream << (int)vec2.x << "; " << (int)vec2.y;
    return stream;
}