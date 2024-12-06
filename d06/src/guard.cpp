#include "guard.h"

auto Guard::turn() noexcept -> void {
    switch (direction) {
        case Up:
            direction = Right;;
            break;
        case Down:
            direction = Left;
            break;
        case Left:
            direction = Up;
            break;
        case Right:
            direction = Down;
            break;
    }
}

[[nodiscard]] auto Guard::get_front_coordinate() const -> std::pair<int, int> {
    auto signed_x_pos = static_cast<int>(position.x);
    auto signed_y_pos = static_cast<int>(position.y);

    switch (direction) {
        case Up:
            signed_y_pos -= 1;
            break;
        case Down:
            signed_y_pos += 1;
            break;
        case Left:
            signed_x_pos -= 1;
            break;
        case Right:
            signed_x_pos += 1;
            break;
    }

    return {signed_x_pos, signed_y_pos};
}

auto Guard::forward() noexcept -> void {
    switch (direction) {
        case Up:
            position.y -= 1;
            break;
        case Down:
            position.y += 1;
            break;
        case Left:
            position.x -= 1;
            break;
        case Right:
            position.x += 1;
            break;
    }
}