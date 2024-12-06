#include <array>
#include <iostream>
#include <fstream>
#include <utility>

#include "guard.h"

namespace {
    constexpr auto height = 130;
    constexpr auto width = 130;
    using RoomLayout = std::array<std::array<std::pair<bool, bool>, width>, height>;

    auto interpret_position(
        const char obstacle,
        const Position& current_position,
        std::pair<bool, bool>& layout_position,
        Position& guard_position,
        Direction& guard_direction
    ) {
        switch (obstacle) {
            case '#':
                layout_position = {true, false};
                break;
            case '.':
                layout_position = {false, false};
                break;
            case '^':
                guard_direction = Up;
                guard_position = current_position;
                break;
            case 'v':
                guard_direction = Down;
                guard_position = current_position;
                break;
            case '>':
                guard_direction = Right;
                guard_position = current_position;
                break;
            case '<':
                guard_direction = Left;
                guard_position = current_position;
                break;
            default:
                throw std::runtime_error("Unexpected character");
        }
    }

    [[nodiscard]] auto read_layout_from_file(
        const std::string& file_path
    ) -> std::pair<RoomLayout, Guard> {
        RoomLayout layout;
        Position guard_position;
        Direction guard_direction;

        auto file = std::ifstream(file_path);
        std::size_t y = 0;
        for (std::string line; std::getline(file, line); ++y) {
            std::size_t x = 0;
            for (const auto c : line) {
                const Position position = {x, y};
                interpret_position(c, position, layout[y][x], guard_position, guard_direction);
                ++x;
            }
        }

        const auto guard = Guard(guard_direction, guard_position);
        return {layout, guard};
    }
}

auto main() -> int {
    const auto file_path = "input.txt";
    const auto [layout, guard] = read_layout_from_file(file_path);
    return 0;
}
