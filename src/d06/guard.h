#ifndef GUARD_H
#define GUARD_H

#include <utility>

struct Position {
    std::size_t x = 0;
    std::size_t y = 0;

    bool operator==(const Position& rhs) const noexcept = default;
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
    auto turn() noexcept -> void;
    auto forward() noexcept -> void;

    bool operator==(const Guard& rhs) const noexcept = default;
};

#endif //GUARD_H
