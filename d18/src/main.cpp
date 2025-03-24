#include "coordinates.h"

#include <iostream>
#include <array>
#include <queue>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cassert>

namespace {
    enum SpaceStatus {
        Safe [[maybe_unused]],
        Corrupted
    };
    using MemorySpace = std::array<std::array<SpaceStatus, max_height>, max_width>;

    struct SpaceRecord {
        bool is_pathed = false;
        unsigned int path_length = 0;
        std::unordered_set<Coordinates, CoordinatesHash> previous_spaces;
    };

    using PathRecords = std::array<std::array<SpaceRecord, max_height>, max_width>;

    struct BestPath {
        const std::unordered_map<Coordinates, unsigned int, CoordinatesHash> path_coords;
        const std::vector<unsigned int> spaces_by_depth;
    };

    [[nodiscard]] auto read_falling_bytes_from_file(const std::string &file_path) -> std::queue<Coordinates> {
        std::queue<Coordinates> falling_bytes;

        auto file = std::ifstream(file_path);
        for (std::string line; std::getline(file, line);) {
            constexpr auto delimiter = ',';
            const auto delimiter_pos = line.find_first_of(delimiter);
            const auto x_coord = std::stoul(line.substr(0, delimiter_pos));
            const auto y_coord = std::stoul(line.substr(delimiter_pos + 1));

            falling_bytes.emplace(x_coord, y_coord);
        }

        return falling_bytes;
    }

    auto simulate_falling_byte(std::queue<Coordinates> &falling_bytes, MemorySpace &memory_space) {
        const auto [x_pos, y_pos] = falling_bytes.front();
        falling_bytes.pop();

        memory_space[x_pos][y_pos] = Corrupted;
    }

    [[nodiscard]] auto collate_best_path_details(
        const unsigned int path_depth,
        const PathRecords &path_records
    ) -> BestPath {
        std::unordered_map<Coordinates, unsigned int, CoordinatesHash> path_coords;
        std::vector<unsigned int> spaces_by_depth(path_depth + 1, 0);

        std::queue<std::pair<Coordinates, unsigned int> > path_queue;
        const Coordinates start_coordinate = {0, 0};
        const Coordinates end_coordinate = {max_width - 1, max_height - 1};
        path_queue.emplace(end_coordinate, path_depth);
        while (not path_queue.empty()) {
            const auto [curr_coordinate, depth] = path_queue.front();
            path_queue.pop();

            path_coords.emplace(curr_coordinate, depth);
            spaces_by_depth[depth] += 1;

            if (curr_coordinate == start_coordinate) {
                return {path_coords, spaces_by_depth};
            }

            for (const auto &prev_space: path_records[curr_coordinate.x][curr_coordinate.y].previous_spaces) {
                if (path_queue.front().first == prev_space) {
                    continue;
                }

                path_queue.emplace(prev_space, depth - 1);
            }
        }

        assert(false and "Failed to unwind best path");
    }

    [[nodiscard]] auto calculate_best_path(const MemorySpace &memory_space) -> std::pair<unsigned int, BestPath> {
        PathRecords path_records;
        path_records[0][0].is_pathed = true;

        auto coordinate_queue = std::queue<Coordinates>{};

        const auto queue_adjacent_spaces = [&](const Coordinates &coordinate) {
            for (const auto adjacent_coord: coordinate.adjacent_coordinates()) {
                const auto [adjacent_x, adjacent_y] = adjacent_coord;
                auto &[is_pathed, path_length, prev_spaces] = path_records[adjacent_x][adjacent_y];

                if (is_pathed or memory_space[adjacent_x][adjacent_y] == Corrupted) {
                    continue;
                }

                is_pathed = true;
                path_length = path_records[coordinate.x][coordinate.y].path_length + 1;
                prev_spaces.emplace(coordinate);

                coordinate_queue.push(adjacent_coord);

                if (adjacent_x == max_width - 1 and adjacent_y == max_height - 1) {
                    std::queue<Coordinates> empty_queue;
                    std::swap(coordinate_queue, empty_queue);
                    break;
                }
            }
        };

        const auto start_pos = Coordinates{0, 0};
        queue_adjacent_spaces(start_pos);
        while (not coordinate_queue.empty()) {
            const auto curr_coordinate = coordinate_queue.front();
            coordinate_queue.pop();
            queue_adjacent_spaces(curr_coordinate);
        }

        const auto path_depth = path_records[max_width - 1][max_height - 1].path_length;
        if (path_depth == 0) {
            throw std::runtime_error("No path found to exit");
        }

        const auto best_path = collate_best_path_details(path_depth, path_records);
        return {path_depth, best_path};
    }

    [[nodiscard]] auto find_blocking_coordinate(
        std::queue<Coordinates> &falling_bytes,
        const BestPath &best_path
    ) -> Coordinates {
        auto num_spaces_by_depth = best_path.spaces_by_depth;
        auto path_coords = best_path.path_coords;

        while (not falling_bytes.empty()) {
            const auto new_byte_position = falling_bytes.front();
            falling_bytes.pop();


            if (not path_coords.contains(new_byte_position)) {
                continue;
            }

            const auto coord_depth = path_coords.at(new_byte_position);
            if (num_spaces_by_depth[coord_depth] == 1) {
                return new_byte_position;
            }

            if (path_coords.erase(new_byte_position) == 0) {
                throw std::runtime_error("Could not remove blocked coordinate from path list");
            }
            num_spaces_by_depth[coord_depth]--;
        }

        throw std::runtime_error("No falling bytes block the best path");
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    auto falling_bytes = read_falling_bytes_from_file(file_path);

    MemorySpace memory_space{};
    unsigned int bytes_simulated = 0;
    for (constexpr auto bytes_to_simulate = 1024; bytes_simulated < bytes_to_simulate; ++bytes_simulated) {
        simulate_falling_byte(falling_bytes, memory_space);
    }

    const auto [min_path_value, best_path] = calculate_best_path(memory_space);
    std::cout << "Minimum path length: " << min_path_value << '\n';

    const auto [block_x, block_y] = find_blocking_coordinate(falling_bytes, best_path);
    std::cout << "Blocking coordinate at " << block_x << ',' << block_y << '\n';

    return 0;
}
