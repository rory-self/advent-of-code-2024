#ifndef GUARD_H
#define GUARD_H

#include <utility>

struct Position {
    std::size_t x = 0;
    std::size_t y = 0;
};

enum Direction {
    Up,
    Down,
    Left,
    Right
};

class Guard {
    Direction direction;
    Position position;

public:
    Guard(const Direction direction, const Position& position)
        : direction{direction}
        , position{position} {}

    [[nodiscard]] auto get_front_coordinate() const -> std::pair<int, int>;

    auto turn() noexcept;
    auto forward();
};

#endif //GUARD_H
