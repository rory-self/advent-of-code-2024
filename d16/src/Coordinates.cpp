#include "Coordinates.h"

auto Coordinates::operator+(const Direction direction) const noexcept -> Coordinates {
    auto new_coords = *this;

    switch (direction) {
        case North:
            new_coords.y -= 1;
            break;
        case South:
            new_coords.y += 1;
            break;
        case West:
            new_coords.x -= 1;
            break;
        case East:
            new_coords.x += 1;
            break;
    }
    return new_coords;
}
