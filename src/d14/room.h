#ifndef ROOM_H
#define ROOM_H

#include <cstddef>

enum Quadrant {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    NoQuadrant
};

struct Coordinates {
    int x;
    int y;

    auto operator+=(const Coordinates &other) noexcept -> Coordinates&;
    [[nodiscard]] auto operator+(const Coordinates& rhs) const -> Coordinates;
    [[nodiscard]] auto operator==(const Coordinates &other) const noexcept -> bool = default;

    [[nodiscard]] auto get_quadrant() const noexcept -> Quadrant;
};

struct CoordinatesHash {
    [[nodiscard]] auto operator()(const Coordinates &coords) const -> std::size_t;
};

#endif //ROOM_H
