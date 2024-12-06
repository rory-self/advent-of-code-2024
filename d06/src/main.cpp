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
                layout_position = {false, true};
                guard_direction = Up;
                guard_position = current_position;
                break;
            case 'v':
                layout_position = {false, true};
                guard_direction = Down;
                guard_position = current_position;
                break;
            case '>':
                layout_position = {false, true};
                guard_direction = Right;
                guard_position = current_position;
                break;
            case '<':
                layout_position = {false, true};
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
    auto [layout, guard] = read_layout_from_file(file_path);

    auto num_visited = 0;

    auto [next_x_pos, next_y_pos] = guard.get_front_coordinate();
    while (next_x_pos >= 0 and next_y_pos >= 0 and next_x_pos < width and next_y_pos < height) {
        if (auto& [is_obstacle, visited] = layout[next_y_pos][next_x_pos]; is_obstacle) {
            guard.turn();
        } else {
            if (visited == false) {
                ++num_visited;
            }

            guard.forward();
            visited = true;
        }

        std::tie(next_x_pos, next_y_pos) = guard.get_front_coordinate();
    }

    std::cout << num_visited << '\n';
    return 0;
}
