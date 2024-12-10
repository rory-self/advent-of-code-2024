#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <array>
#include <queue>

namespace {
    constexpr auto map_height = 60;
    constexpr auto map_width = 60;
    using TopographicMap = std::array<std::array<uint8_t, map_width>, map_height>;

    struct Coordinates {
        std::size_t x;
        std::size_t y;
    };

    [[nodiscard]] auto read_map_from_file(const std::string& file_path) -> std::pair<TopographicMap, std::vector<Coordinates>> {
        auto file = std::ifstream(file_path);
        TopographicMap topo_map;
        std::vector<Coordinates> trailhead_coords;

        std::size_t row = 0;
        for (std::string line; std::getline(file, line);) {
            std::size_t col = 0;
            for (const auto& c : line) {
                const uint8_t height = c - '0';
                if (height == 0) {
                    trailhead_coords.emplace_back(Coordinates{col, row});
                }

                topo_map[row][col] = height;
                ++col;
            }
            ++row;
        }
        return {topo_map, trailhead_coords};
    }

    [[nodiscard]] auto calculate_trailhead_score(const TopographicMap& topo_map, const Coordinates& coordinates) -> int {
        constexpr std::pair<int, int> directions[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        auto coordinates_queue = std::queue<Coordinates>{{coordinates}};

        const auto within_bounds = [](const int x, const int y) {
            return x >= 0 && y >= 0 && x < map_width && y < map_height;
        };

        auto trailhead_score = 0;
        while (not coordinates_queue.empty()) {
            const auto [curr_x, curr_y] = coordinates_queue.front();
            coordinates_queue.pop();

            const auto curr_height = topo_map[curr_y][curr_x];
            for (const auto& [x_diff, y_diff] : directions) {
                const auto x = static_cast<int>(curr_x) + x_diff;
                const auto y = static_cast<int>(curr_y) + y_diff;

                if (not within_bounds(x, y)) {
                    continue;
                }

                const auto adjacent_height = topo_map[y][x];
                if (constexpr auto max_height = 9; adjacent_height == max_height) {
                    trailhead_score += 1;
                } else if (curr_height + 1 == adjacent_height) {
                    coordinates_queue.push(Coordinates{
                        static_cast<std::size_t>(x),
                        static_cast<std::size_t>(y)
                    });
                }
            }
        }

        return trailhead_score;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto [topo_map, trailhead_coords] = read_map_from_file(file_path);

    auto score_sum = 0;
    for (const auto& coords : trailhead_coords) {
        score_sum += calculate_trailhead_score(topo_map, coords);
    }

    std::cout << score_sum << '\n';
    return 0;
}