#include <regex>

#include "robot.h"

namespace {
    [[nodiscard]] auto extract_coordinates(const std::string &coordinate_parameters) -> Coordinates {
        constexpr auto delimiter = ',';
        const auto delimiter_pos = coordinate_parameters.find(delimiter);

        const auto x_param = coordinate_parameters.substr(0, delimiter_pos);
        const auto y_param = coordinate_parameters.substr(delimiter_pos + 1);

        const auto x_int = std::stoi(x_param);
        const auto y_int = std::stoi(y_param);
        return {x_int, y_int};
    }
}

Robot::Robot(const Coordinates &position, const Coordinates &velocity)
    : position(position)
    , velocity(velocity) {};

auto Robot::from_string(const std::string &parameter_line) -> Robot {
    const auto parameter_pattern = std::regex{"[\\-0-9]+,[\\-0-9]+"};
    auto regex_it = std::regex_iterator(parameter_line.cbegin(), parameter_line.cend(), parameter_pattern);

    const auto position_string = regex_it->str();
    ++regex_it;
    const auto velocity_string = regex_it->str();

    const auto position = extract_coordinates(position_string);
    const auto velocity = extract_coordinates(velocity_string);

    return {position, velocity};
}


auto Robot::move() noexcept -> void {
    position += velocity;
}

auto Robot::get_quadrant() const noexcept -> Quadrant {
    return position.get_quadrant();
}
