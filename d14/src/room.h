#ifndef ROOM_H
#define ROOM_H

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
    [[nodiscard]] auto get_quadrant() const noexcept -> Quadrant;
};

#endif //ROOM_H
