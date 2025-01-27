#include "coordinates.h"

#include <iostream>
#include <array>
#include <queue>
#include <string>
#include <fstream>

namespace {
    enum SpaceStatus {
        Safe,
        Corrupted
    };

    struct Space {
        SpaceStatus status = Safe;
        bool is_pathed = false;
        unsigned int path_length = 0;
    };

    using MemorySpace = std::array<std::array<Space, max_height>, max_width>;

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

        memory_space[x_pos][y_pos].status = Corrupted;
    }

    [[nodiscard]] auto calculate_min_path(MemorySpace &memory_space) -> unsigned int {
        auto coordinate_queue = std::queue<Coordinates>{};

        const auto queue_adjacent_spaces = [&](const Coordinates &coordinate) {
            for (const auto adjacent_coord: coordinate.adjacent_coordinates()) {
                const auto [adjacent_x, adjacent_y] = adjacent_coord;
                auto &[status, is_pathed, path_length] = memory_space[adjacent_x][adjacent_y];

                if (is_pathed or status == Corrupted) {
                    continue;
                }

                is_pathed = true;
                path_length = memory_space[coordinate.x][coordinate.y].path_length + 1;

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

        const auto min_path = memory_space[max_width - 1][max_height - 1].path_length;
        if (min_path == 0) {
            throw std::runtime_error("No path found to exit");
        }
        return min_path;
    }
}

auto main() -> int {
    auto memory_space = MemorySpace{};
    memory_space[0][0].is_pathed = true;

    const auto file_path = std::string{"input.txt"};
    auto falling_bytes = read_falling_bytes_from_file(file_path);

    constexpr auto bytes_to_simulate = 1024;
    for (auto i = 0; i < bytes_to_simulate; ++i) {
        simulate_falling_byte(falling_bytes, memory_space);
    }

    const auto min_path_value = calculate_min_path(memory_space);
    std::cout << min_path_value << '\n';

    return 0;
}
