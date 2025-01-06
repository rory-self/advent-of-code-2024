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

    [[nodiscard]] auto read_maze_from_file(const std::string &filename) -> std::pair<Maze, Coordinates> {
        Maze maze;
        auto file = std::ifstream{filename};

        Coordinates starting_coords{};
        std::size_t y = 0;
        for (std::string line; std::getline(file, line);) {
            std::size_t x = 0;
            for (const auto c: line) {
                const auto entity = char_to_entity(c);
                maze[y][x] = {entity, 0, false};

                if (constexpr auto start_char = 'S'; c == start_char) {
                    starting_coords = Coordinates{x, y};
                    maze[y][x].visited = true;
                }
                ++x;
            }
            ++y;
        }

        return {maze, starting_coords};
    }

    [[nodiscard]] auto calculate_least_points(Maze &maze, const Coordinates &starting_pos) -> unsigned int {
        unsigned int least_points = 0;
        std::queue<std::pair<Coordinates, Direction>> coord_queue;
        coord_queue.emplace(starting_pos, East);

        const auto add_tile_to_queue = [&coord_queue, &maze](const Coordinates& base_coords, const Direction direction, const unsigned int new_points) {
            const auto new_coords = base_coords + direction;
            const auto [new_x, new_y] = new_coords;
            const auto [new_entity, min_score, new_tile_visited] = maze[new_y][new_x];
            if (new_entity != Wall and (not new_tile_visited or min_score > new_points)) {
                maze[new_y][new_x].visited = true;
                maze[new_y][new_x].min_score = new_points;
                coord_queue.emplace(new_coords, direction);
            }
        };

        add_tile_to_queue(starting_pos, West, 2001);

        while (not coord_queue.empty()) {
            const auto [curr_coord, curr_direction] = coord_queue.front();
            coord_queue.pop();

            const auto [curr_entity, curr_score, _] = maze[curr_coord.y][curr_coord.x];
            if (curr_entity == End) {
                if (least_points == 0 or least_points > curr_score) {
                    least_points = curr_score;
                }
                continue;
            }

            const auto [forward, left, right] = get_relative_directions(curr_direction);
            constexpr unsigned int points_to_turn = 1001;
            add_tile_to_queue(curr_coord, forward, curr_score + 1);
            add_tile_to_queue(curr_coord, left, curr_score + points_to_turn);
            add_tile_to_queue(curr_coord, right, curr_score + points_to_turn);
        }

        return least_points;
    }
}

auto main() -> int {
    const auto filename = std::string{"input.txt"};
    auto [maze, reindeer] = read_maze_from_file(filename);

    const auto num_points = calculate_least_points(maze, reindeer);
    std::cout << num_points << '\n';
    return 0;
}
