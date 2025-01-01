#ifndef REINDEER_H
#define REINDEER_H

#include "Coordinates.h"

#include <array>

class Reindeer {
    Direction direction = Right;
    Coordinates coordinates{};

public:
    Reindeer() = default;

    auto update_coordinates(const Coordinates &new_coordinates) {
        coordinates = new_coordinates;
    }

    auto update_direction(const Direction &new_direction) {
        direction = new_direction;
    }

    [[nodiscard]] auto get_coordinates() const noexcept -> Coordinates;

    [[nodiscard]] auto get_relative_directions() const -> std::array<Direction, 4>;
};


#endif //REINDEER_H
