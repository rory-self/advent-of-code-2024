#include "Reindeer.h"

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

    [[nodiscard]] auto get_opposite_direction(const Direction direction) -> Direction {
        switch (direction) {
            case East:
                return West;
            case West:
                return East;
            case South:
                return North;
            case North:
                return South;
        }
    }
}

auto Reindeer::get_relative_directions() const -> std::array<Direction, 4> {
    std::array<Direction, 4> directions{};
    directions[0] = direction;
    directions[1] = get_direction_to_left(direction);
    directions[2] = get_direction_to_right(direction);
    directions[3] = get_opposite_direction(direction);

    return directions;
}

auto Reindeer::get_coordinates() const noexcept -> Coordinates {
    return coordinates;
}

