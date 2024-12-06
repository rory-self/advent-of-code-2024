#include "Guard.h"

auto Guard::turn() noexcept {
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
    const auto signed_x_pos = static_cast<int>(position.x);
    const auto signed_y_pos = static_cast<int>(position.y);

    switch (direction) {
        case Up:
            break;
        case Down:
            break;
        case Left:
            break;
        case Right:
            break;
        }
    }

auto Guard::forward() {
    switch (direction) {
        case Up:
            break;
        case Down:
            break;
        case Left:
            break;
        case Right:
            break;
        }
    }