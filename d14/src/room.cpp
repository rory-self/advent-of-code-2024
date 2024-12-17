#include "room.h"

constexpr auto room_width = 101;
constexpr auto room_height = 103;

auto Coordinates::operator+=(const Coordinates &other) noexcept -> Coordinates& {
    *this = *this + other;
    return *this;
}

auto Coordinates::operator+(const Coordinates& rhs) const -> Coordinates {
    const auto wrap_around = [](const int coordinate_sum, const int limit) -> int {
        if (coordinate_sum >= limit) {
            return coordinate_sum - limit;
        }
        if (coordinate_sum < 0) {
            return coordinate_sum + limit;
        }
        return coordinate_sum;
    };

    const auto new_x = x + rhs.x;
    const auto wrapped_x = wrap_around(new_x, room_width);

    const auto new_y = y + rhs.y;
    const auto wrapped_y = wrap_around(new_y, room_height);
    return {wrapped_x, wrapped_y};
}

auto Coordinates::get_quadrant() const noexcept -> Quadrant {
    const auto is_top = y < room_height / 2;
    const auto is_left = x < room_width / 2 ;
    const auto is_right = x > room_width / 2;
    const auto is_bottom = y > room_height / 2;

    if (is_top && is_left) {
        return TopLeft;
    }
    if (is_bottom && is_left) {
        return BottomLeft;
    }
    if (is_top && is_right) {
        return TopRight;
    }
    if (is_bottom && is_right) {
        return BottomRight;
    }
    return NoQuadrant;
}

auto CoordinatesHash::operator()(const Coordinates &coords) const -> std::size_t {
    const auto x_hash = static_cast<size_t>(coords.x);
    const auto y_hash = static_cast<size_t>(coords.y);

    return x_hash ^ y_hash << 1;
}

