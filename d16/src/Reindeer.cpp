#include "Reindeer.h"

namespace {
    [[nodiscard]] auto get_direction_to_left(const Direction direction) -> Direction {
        switch (direction) {
            case Up:
                return Left;
            case Down:
                return Right;
            case Right:
                return Up;
            case Left:
                return Down;
        }
    }

    [[nodiscard]] auto get_direction_to_right(const Direction direction) -> Direction {
        switch (direction) {
            case Down:
                return Left;
            case Up:
                return Right;
            case Left:
                return Up;
            case Right:
                return Down;
        }
    }

    [[nodiscard]] auto get_opposite_direction(const Direction direction) -> Direction {
        switch (direction) {
            case Right:
                return Left;
            case Left:
                return Right;
            case Down:
                return Up;
            case Up:
                return Down;
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

