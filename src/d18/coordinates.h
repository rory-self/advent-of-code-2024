#ifndef COORDINATES_H
#define COORDINATES_H

#include <cstddef>
#include <vector>
#include <unordered_set>

constexpr auto max_height = 71;
constexpr auto max_width = 71;

enum Direction {
    Left,
    Right,
    Down,
    Up
};

struct Coordinates {
    std::size_t x;
    std::size_t y;

    Coordinates(std::size_t x, std::size_t y);

    [[nodiscard]] auto adjacent_coordinates() const noexcept -> std::vector<Coordinates>;

    [[nodiscard]] auto operator==(const Coordinates& other) const noexcept -> bool;

private:
    [[nodiscard]] auto available_directions() const noexcept -> std::unordered_set<Direction>;

    [[nodiscard]] auto operator+(Direction direction) const noexcept -> Coordinates;
};

struct CoordinatesHash {
    [[nodiscard]] auto operator()(const Coordinates& coords) const noexcept -> std::size_t;
};


#endif //COORDINATES_H
