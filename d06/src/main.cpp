#include <array>
#include <iostream>
#include <fstream>
#include <utility>
#include <ranges>

#include "guard.h"

namespace {
    constexpr auto height = 130;
    constexpr auto width = 130;
    using RoomLayout = std::array<std::array<std::pair<bool, bool>, width>, height>;

    auto interpret_position(
        const char obstacle,
        const Position& current_position,
        std::pair<bool, bool>& position_data,
        Position& guard_position,
        Direction& guard_direction
    ) {
        switch (obstacle) {
            case '#':
                position_data = {true, false};
                break;
            case '.':
                position_data = {false, false};
                break;
            case '^':
                position_data = {false, true};
                guard_direction = Up;
                guard_position = current_position;
                break;
            case 'v':
                position_data = {false, true};
                guard_direction = Down;
                guard_position = current_position;
                break;
            case '>':
                position_data = {false, true};
                guard_direction = Right;
                guard_position = current_position;
                break;
            case '<':
                position_data = {false, true};
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

    [[nodiscard]] auto count_path_length(RoomLayout layout, Guard guard) -> unsigned int {
        auto path_length = 1;

        auto [next_x_pos, next_y_pos] = guard.get_front_coordinate();
        while (next_x_pos >= 0 and next_y_pos >= 0 and next_x_pos < width and next_y_pos < height) {
            if (auto& [is_obstacle, visited] = layout[next_y_pos][next_x_pos]; is_obstacle) {
                guard.turn();
            } else {
                if (not visited) {
                    ++path_length;
                }

                guard.forward();
                visited = true;
            }

            std::tie(next_x_pos, next_y_pos) = guard.get_front_coordinate();
        }

        return path_length;
    }

    [[nodiscard]] auto is_invalid_path(const RoomLayout &layout, Guard guard) -> bool {
        auto fast_guard = guard;

        const auto move_guard = [](const RoomLayout& l, Guard& g) {
            const auto out_of_bounds = [](const int x, const int y) {
                return x < 0 or y < 0 or x > width or y > height;
            };

            const auto [next_x, next_y] = g.get_front_coordinate();
            if (out_of_bounds(next_x, next_y)) {
                return false;
            }

            auto is_obstacle =  l[next_y][next_x].first;
            while (is_obstacle) {
                g.turn();
                const auto [next_x, next_y] = g.get_front_coordinate();

                if (out_of_bounds(next_x, next_y)) {
                    return false;
                }
                is_obstacle =  l[next_y][next_x].first;
            }

            g.forward();
            return true;
        };

        do {
            if (not move_guard(layout, guard)) {
                return false;
            }

            if (not move_guard(layout, fast_guard)) {
                return false;
            }

            if (not move_guard(layout, fast_guard)) {
                return false;
            }
        } while (fast_guard.get_position() != guard.get_position());
        return true;
    }

    [[nodiscard]] auto count_blocking_obstructions(RoomLayout& layout, const Guard& guard) -> unsigned int {
        auto num_obstructions = 0;
        for (auto& y_it : layout) {
            for (auto& is_obstacle : y_it | std::views::keys) {
                if (is_obstacle) {
                    continue;
                }
                is_obstacle = true;

                if (is_invalid_path(layout, guard)) {
                    ++num_obstructions;
                }
                is_obstacle = false;
            }
        }

        return num_obstructions;
    }
}

auto main() -> int {
    const auto file_path = "input.txt";
    auto [layout, guard] = read_layout_from_file(file_path);

    const auto num_visited = count_path_length(layout, guard);
    const auto num_obstructions = count_blocking_obstructions(layout, guard);

    std::cout << num_visited << '\n';
    std::cout << num_obstructions << '\n';
    return 0;
}
