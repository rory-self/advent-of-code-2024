#ifndef COORDINATES_H
#define COORDINATES_H

#include <cstddef>
#include <array>

enum Direction {
    North,
    South,
    West,
    East
};

struct Coordinates {
    std::size_t x;
    std::size_t y;

    Coordinates() = default;

    Coordinates(const std::size_t x, const std::size_t y): x(x), y(y) {
    }

    [[nodiscard]] auto operator==(const Coordinates& other) const noexcept -> bool = default;
    auto operator=(const Coordinates &other) noexcept -> Coordinates & = default;
    auto operator+(Direction direction) const noexcept -> Coordinates;
    auto operator-(Direction direction) const noexcept -> Coordinates;
};

[[nodiscard]] auto get_relative_directions(Direction direction) -> std::array<Direction, 3>;

#endif //COORDINATES_H
