#ifndef ROBOT_H
#define ROBOT_H

#include <string>

#include "room.h"

class Robot {
    Coordinates position;
    const Coordinates velocity;

    Robot(const Coordinates& position, const Coordinates& velocity);
public:
    [[nodiscard]] static auto from_string(const std::string& parameter_line) -> Robot;

    auto move() noexcept -> void;
    [[nodiscard]] auto get_quadrant() const noexcept -> Quadrant;
    [[nodiscard]] auto get_position() const noexcept -> Coordinates;
};

#endif //ROBOT_H
