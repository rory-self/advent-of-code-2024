#include "Coordinates.h"

#include <iostream>
#include <array>
#include <fstream>
#include <queue>

namespace {
    enum Entity {
        Wall,
        End,
        Empty,
    };

    struct Tile {
        Entity entity;
        unsigned int min_score;
    };

    constexpr std::size_t maze_height = 142;
    constexpr std::size_t maze_width = 142;
    using Maze = std::array<std::array<Tile, maze_width>, maze_height>;

    [[nodiscard]] auto char_to_entity(const char c) -> Entity {
        switch (c) {
            case 'S':
            case '.':
                return Empty;
            case '#':
                return Wall;
            case 'E':
                return End;
            default:
                throw std::invalid_argument("Invalid entity character provided");
        }
    }

    [[nodiscard]] auto read_maze_from_file(const std::string& filename) -> std::pair<Maze, Coordinates> {
        Maze maze;
        auto file = std::ifstream{filename};

        auto start_position = Coordinates();
        std::size_t y = 0;
        for (std::string line; std::getline(file, line);) {
            std::size_t x = 0;
            for (const auto& c : line) {
                if (constexpr auto start_char = 'S'; c == start_char) {
                    start_position = Coordinates{x, y};
                }

                const auto entity = char_to_entity(c);
                maze[y][x] = {entity, 0};
                ++x;
            }
            ++y;
        }

        return {maze, start_position};
    }

    [[nodiscard]] auto calculate_least_points(const Maze& maze, const Coordinates& start_pos) -> unsigned int {
        unsigned int points = 0;

        std::queue<Coordinates> coord_queue;
        coord_queue.push(start_pos);

        auto curr_direction = Direction::Right;
        while (not coord_queue.empty()) {
            const auto [curr_x, curr_y] = coord_queue.front();
            coord_queue.pop();
        }

        return points;
    }
}

auto main() -> int {
    const auto filename = std::string{"input.txt"};
    const auto [maze, start_pos] = read_maze_from_file(filename);

    const auto num_points = calculate_least_points(maze, start_pos);
    std::cout << num_points << '\n';
    return 0;
}
