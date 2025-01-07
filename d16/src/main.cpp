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
        std::array<bool, 4> direction_visited_from;
    };

    constexpr std::size_t maze_height = 15;
    constexpr std::size_t maze_width = 15;
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

    [[nodiscard]] auto read_maze_from_file(const std::string &filename) -> std::tuple<Maze, Coordinates, Coordinates> {
        Maze maze;
        auto file = std::ifstream{filename};

        Coordinates starting_coords{};
        Coordinates end_coords{};
        std::size_t y = 0;
        for (std::string line; std::getline(file, line);) {
            std::size_t x = 0;
            for (const auto c: line) {
                const auto entity = char_to_entity(c);
                maze[y][x] = {entity, 0, false, {false, false, false, false}};

                if (constexpr auto start_char = 'S'; c == start_char) {
                    starting_coords = Coordinates{x, y};
                    maze[y][x].visited = true;
                    maze[y][x].direction_visited_from = {true, true, true, true};
                } else if (constexpr auto end_char = 'E'; c == end_char) {
                    end_coords = Coordinates{x, y};
                }
                ++x;
            }
            ++y;
        }

        return {maze, starting_coords, end_coords};
    }

    [[nodiscard]] auto calculate_least_points(Maze &maze, const Coordinates &starting_pos) -> unsigned int {
        unsigned int least_points = 0;
        std::queue<std::pair<Coordinates, Direction> > coord_queue;
        coord_queue.emplace(starting_pos, East);

        const auto add_tile_to_queue = [&coord_queue, &maze](const Coordinates &base_coords, const Direction direction,
                                                             const unsigned int new_points) {
            const auto new_coords = base_coords + direction;
            const auto [new_x, new_y] = new_coords;
            const auto [new_entity, min_score, new_tile_visited, _] = maze[new_y][new_x];
            if (new_entity != Wall and (not new_tile_visited or min_score >= new_points)) {
                if (new_points < min_score) {
                    maze[new_y][new_x].direction_visited_from = {false, false, false, false};
                }

                maze[new_y][new_x].direction_visited_from[direction] = true;
                maze[new_y][new_x].visited = true;
                maze[new_y][new_x].min_score = new_points;
                coord_queue.emplace(new_coords, direction);
            }
        };

        add_tile_to_queue(starting_pos, West, 2001);

        while (not coord_queue.empty()) {
            const auto [curr_coord, curr_direction] = coord_queue.front();
            coord_queue.pop();

            const auto [curr_entity, curr_score, _visited, _] = maze[curr_coord.y][curr_coord.x];
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

    [[nodiscard]] auto calculate_tile_num_on_path(const Maze &maze, const Coordinates &end_pos,
                                                  const Coordinates &start_pos) -> unsigned int {
        unsigned int num_tiles = 0;

        auto coord_queue = std::queue<Coordinates>{};
        coord_queue.push(end_pos);

        while (not coord_queue.empty()) {
            num_tiles += 1;

            const auto curr_coords = coord_queue.front();
            coord_queue.pop();

            if (curr_coords == start_pos) {
                continue;
            }

            const auto preceding_directions = maze[curr_coords.y][curr_coords.x].direction_visited_from;
            for (auto i = 0; i < preceding_directions.size(); ++i) {
                if (not preceding_directions[i]) {
                    continue;
                }
                const auto direction = static_cast<Direction>(i);
                const auto preceding_coordinate = curr_coords - direction;
                coord_queue.push(preceding_coordinate);
            }
        }

        return num_tiles;
    }
}

auto main() -> int {
    const auto filename = std::string{"test.txt"};
    auto [maze, start_pos, end_pos] = read_maze_from_file(filename);

    const auto num_points = calculate_least_points(maze, start_pos);
    std::cout << "Least points: " << num_points << '\n';

    const auto num_tiles = calculate_tile_num_on_path(maze, end_pos, start_pos);
    std::cout << "Tiles on best path(s): " << num_tiles << '\n';
    return 0;
}
