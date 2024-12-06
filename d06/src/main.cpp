#include <array>
#include <iostream>
#include <fstream>
#include <utility>
#include <ranges>

#include "guard.h"

namespace {
    constexpr auto height = 130;
    constexpr auto width = 130;

    enum PositionState {
        Empty,
        Obstacle,
        Visited,
    };
    using RoomLayout = std::array<std::array<PositionState, width>, height>;


    auto interpret_position(
        const char obstacle,
        const Position& current_position,
        PositionState& position_data,
        Position& guard_position,
        Direction& guard_direction
    ) {
        switch (obstacle) {
            case '#':
                position_data = Obstacle;
                break;
            case '.':
                position_data = Empty;
                break;
            case '^':
                position_data = Visited;
                guard_direction = Up;
                guard_position = current_position;
                break;
            case 'v':
                position_data = Visited;
                guard_direction = Down;
                guard_position = current_position;
                break;
            case '>':
                position_data = Visited;
                guard_direction = Right;
                guard_position = current_position;
                break;
            case '<':
                position_data = Visited;
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

    [[nodiscard]] auto is_out_of_bounds(const int x, const int y) noexcept -> bool {
        return x < 0 or y < 0 or x > width or y > height;
    }

    [[nodiscard]] auto count_path_length(RoomLayout layout, Guard guard) -> unsigned int {
        auto path_length = 0;

        auto [next_x_pos, next_y_pos] = guard.get_front_coordinate();
        while (not is_out_of_bounds(next_x_pos, next_y_pos)) {
            if (auto& position_info = layout[next_y_pos][next_x_pos]; position_info == Obstacle) {
                guard.turn();
            } else {
                if (position_info != Visited) {
                    ++path_length;
                }

                guard.forward();
                position_info = Visited;
            }

            std::tie(next_x_pos, next_y_pos) = guard.get_front_coordinate();
        }

        return path_length;
    }

    [[nodiscard]] auto is_looping_path(const RoomLayout &layout, Guard guard) -> bool {
        auto fast_guard = guard;

        const auto try_move_guard = [](const RoomLayout& l, Guard& g) {
            auto [next_x, next_y] = g.get_front_coordinate();
            if (is_out_of_bounds(next_x, next_y)) {
                return false;
            }

            while (l[next_y][next_x] == Obstacle) {
                g.turn();
                std::tie(next_x, next_y) = g.get_front_coordinate();

                if (is_out_of_bounds(next_x, next_y)) {
                    return false;
                }
            }

            g.forward();
            return true;
        };

        do {
            if (not try_move_guard(layout, guard)) {
                return false;
            }

            if (not try_move_guard(layout, fast_guard)) {
                return false;
            }

            if (not try_move_guard(layout, fast_guard)) {
                return false;
            }
        } while (fast_guard != guard);
        return true;
    }

    [[nodiscard]] auto count_blocking_obstructions(RoomLayout& layout, const Guard& guard) -> unsigned int {
        auto num_obstructions = 0;
        for (auto& y_it : layout) {
            for (auto& position_info : y_it) {
                if (position_info != Empty) {
                    continue;
                }
                position_info = Obstacle;

                if (is_looping_path(layout, guard)) {
                    ++num_obstructions;
                }
                position_info = Empty;
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
