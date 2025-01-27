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
    using MemorySpace = std::array<std::array<SpaceStatus, max_height>, max_width>;

    struct SpaceRecord {
        bool is_pathed = false;
        unsigned int path_length = 0;
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

    [[maybe_unused]] auto calculate_min_path(MemorySpace &memory_space) -> unsigned int {
        auto path_records = std::array<std::array<SpaceRecord, max_height>, max_width>{};
        path_records[0][0].is_pathed = true;

        auto coordinate_queue = std::queue<Coordinates>{};

        const auto queue_adjacent_spaces = [&](const Coordinates &coordinate) {
            for (const auto adjacent_coord: coordinate.adjacent_coordinates()) {
                const auto [adjacent_x, adjacent_y] = adjacent_coord;
                auto &[is_pathed, path_length] = path_records[adjacent_x][adjacent_y];

                if (is_pathed or memory_space[adjacent_x][adjacent_y] == Corrupted) {
                    continue;
                }

                is_pathed = true;
                path_length = path_records[coordinate.x][coordinate.y].path_length + 1;

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

        const auto min_path = path_records[max_width - 1][max_height - 1].path_length;
        if (min_path == 0) {
            throw std::runtime_error("No path found to exit");
        }
        return min_path;
    }
}

auto main() -> int {
    auto memory_space = MemorySpace{};

    const auto file_path = std::string{"input.txt"};
    auto falling_bytes = read_falling_bytes_from_file(file_path);

    constexpr auto bytes_to_simulate = 1024;
    unsigned int bytes_simulated;
    for (bytes_simulated = 0; bytes_simulated < bytes_to_simulate; ++bytes_simulated) {
        simulate_falling_byte(falling_bytes, memory_space);
    }

    const auto min_path_value = calculate_min_path(memory_space);
    std::cout << "Minimum path length: " << min_path_value << '\n';

    for (;; ++bytes_simulated) {
        const auto [last_byte_x, last_byte_y] = falling_bytes.front();
        simulate_falling_byte(falling_bytes, memory_space);

        try {
            calculate_min_path(memory_space);
        } catch (const std::runtime_error &) {
            std::cout << "Blocking Byte Coordinate: " << last_byte_x << "," << last_byte_y << '\n';
            break;
        }
    }

    return 0;
}
