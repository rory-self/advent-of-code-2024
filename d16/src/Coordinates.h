#ifndef COORDINATES_H
#define COORDINATES_H

#include <cstddef>

enum Direction {
    Up,
    Down,
    Left,
    Right
};

struct Coordinates {
    std::size_t x;
    std::size_t y;

    Coordinates() = default;
    Coordinates(const std::size_t x, const std::size_t y): x(x), y(y) {}

    auto operator=(const Coordinates& other) noexcept -> Coordinates& = default;
    [nodiscard] auto operator+(Direction direction) const noexcept -> Coordinates;
};

#endif //COORDINATES_H
