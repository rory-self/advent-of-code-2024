#include "coordinates.h"

namespace {
    [[nodiscard]] auto get_direction_to_left(const Direction direction) -> Direction {
        switch (direction) {
            case North:
                return West;
            case South:
                return East;
            case East:
                return North;
            case West:
                return South;
        }
    }

    [[nodiscard]] auto get_direction_to_right(const Direction direction) -> Direction {
        switch (direction) {
            case South:
                return West;
            case North:
                return East;
            case West:
                return North;
            case East:
                return South;
        }
    }
}

auto get_relative_directions(const Direction direction) -> std::array<Direction, 3> {
    std::array<Direction, 3> directions{};
    directions[0] = direction;
    directions[1] = get_direction_to_left(direction);
    directions[2] = get_direction_to_right(direction);

    return directions;
}

[[nodiscard]] auto Coordinates::operator+(const Direction direction) const noexcept -> Coordinates {
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

[[nodiscard]] auto Coordinates::operator-(const Direction direction) const noexcept -> Coordinates {
    auto new_coords = *this;

    switch (direction) {
        case North:
            new_coords.y += 1;
        break;
        case South:
            new_coords.y -= 1;
        break;
        case West:
            new_coords.x += 1;
        break;
        case East:
            new_coords.x -= 1;
        break;
    }
    return new_coords;
}
