#include "Coordinates.h"
#include "Reindeer.h"

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
        bool visited;
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

    [[nodiscard]] auto read_maze_from_file(const std::string &filename) -> std::pair<Maze, Reindeer> {
        Maze maze;
        auto file = std::ifstream{filename};

        auto reindeer = Reindeer{};
        std::size_t y = 0;
        for (std::string line; std::getline(file, line);) {
            std::size_t x = 0;
            for (const auto &c: line) {
                if (constexpr auto start_char = 'S'; c == start_char) {
                    const auto start_position = Coordinates{x, y};
                    reindeer.update_coordinates(start_position);
                }

                const auto entity = char_to_entity(c);
                maze[y][x] = {entity, 0, false};
                ++x;
            }
            ++y;
        }

        return {maze, reindeer};
    }

    [[nodiscard]] auto calculate_least_points(const Maze &maze, const Reindeer &reindeer) -> unsigned int {
        unsigned int points = 0;

        std::queue<Coordinates> coord_queue;
        const auto start_pos = reindeer.get_coordinates();
        coord_queue.push(start_pos);

        while (not coord_queue.empty()) {
            const auto [curr_x, curr_y] = coord_queue.front();
            coord_queue.pop();
        }

        return points;
    }
}

auto main() -> int {
    const auto filename = std::string{"input.txt"};
    const auto [maze, reindeer] = read_maze_from_file(filename);

    const auto num_points = calculate_least_points(maze, reindeer);
    std::cout << num_points << '\n';
    return 0;
}
