#include "Coordinates.h"

auto Coordinates::operator+(const Direction direction) const noexcept -> Coordinates {
    auto new_coords = *this;

    switch (direction) {
        Up:
            new_coords.y -= 1;
            break;
        Down:
            new_coords.y += 1;
            break;
        Left:
            new_coords.x -= 1;
            break;
        Right:
            new_coords.x += 1;
            break;
    }
    return new_coords;
}
