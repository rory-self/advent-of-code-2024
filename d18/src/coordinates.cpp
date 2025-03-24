#include "coordinates.h"

#include <cassert>

Coordinates::Coordinates(const std::size_t x, const std::size_t y): x(x), y(y) {
}

auto Coordinates::available_directions() const noexcept -> std::unordered_set<Direction> {
    std::unordered_set<Direction> directions;

    if (x != 0) {
        directions.emplace(Left);
    }
    if (y != 0) {
        directions.emplace(Up);
    }
    if (y != max_height - 1) {
        directions.emplace(Down);
    }
    if (x != max_width - 1) {
        directions.emplace(Right);
    }

    return directions;
}

auto Coordinates::operator==(const Coordinates &other) const noexcept -> bool {
    return x == other.x and y == other.y;
}

auto Coordinates::operator+(const Direction direction) const noexcept -> Coordinates {
    auto new_coordinates = *this;

    switch (direction) {
        case Left:
            new_coordinates.x -= 1;
            break;
        case Up:
            new_coordinates.y -= 1;
            break;
        case Down:
            new_coordinates.y += 1;
            break;
        case Right:
            new_coordinates.x += 1;
            break;
        default:
            assert(false and "unhandled direction argument");
    }

    return new_coordinates;
}

auto Coordinates::adjacent_coordinates() const noexcept -> std::vector<Coordinates> {
    std::vector<Coordinates> coordinates;

    for (const auto available_directions = this->available_directions(); const auto direction: available_directions) {
        const auto adjacent_coordinate = *this + direction;
        coordinates.push_back(adjacent_coordinate);
    }

    return coordinates;
}

auto CoordinatesHash::operator()(const Coordinates& coords) const noexcept -> std::size_t {
    return coords.x ^ coords.y << 1;
}
