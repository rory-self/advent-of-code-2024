#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <array>
#include <functional>
#include <queue>
#include <unordered_set>

namespace {
    constexpr auto map_height = 60;
    constexpr auto map_width = 60;
    using TopographicMap = std::array<std::array<uint8_t, map_width>, map_height>;

    struct Coordinates {
        std::size_t x;
        std::size_t y;

        [[nodiscard]] auto operator==(const Coordinates& other) const -> bool = default;
    };

    struct CoordinatesHasher {
        [[nodiscard]] auto operator()(const Coordinates& coords) const -> std::size_t {
            return coords.x ^ coords.y << 1;
        }
    };

    using CoordinatesSet = std::unordered_set<Coordinates, CoordinatesHasher>;

    [[nodiscard]] auto read_map_from_file(
        const std::string& file_path
    ) -> std::pair<TopographicMap, std::vector<Coordinates>> {
        auto file = std::ifstream(file_path);
        TopographicMap topographic_map;
        std::vector<Coordinates> trailhead_coords;

        std::size_t row = 0;
        for (std::string line; std::getline(file, line); ++row) {
            std::size_t col = 0;
            for (const auto& c : line) {
                const uint8_t height = c - '0';
                if (height == 0) {
                    trailhead_coords.emplace_back(Coordinates{col, row});
                }

                topographic_map[row][col] = height;
                ++col;
            }
        }
        return {topographic_map, trailhead_coords};
    }

    [[nodiscard]] auto calculate_trailhead_metric(
        const TopographicMap& topographic_map,
        const Coordinates& coordinates,
        const std::function<bool(uint&, uint, const Coordinates&, CoordinatesSet&)>& metric_requirements
    ) -> uint {
        constexpr std::pair<int, int> directions[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        auto coordinates_queue = std::queue<Coordinates>{{coordinates}};

        const auto within_bounds = [](const int x, const int y) {
            return x >= 0 and y >= 0 and x < map_width and y < map_height;
        };

        uint trailhead_score = 0;
        CoordinatesSet peaks_reached;
        while (not coordinates_queue.empty()) {
            const auto curr_coords = coordinates_queue.front();
            const auto [curr_x, curr_y] = curr_coords;
            coordinates_queue.pop();

            const auto curr_height = topographic_map[curr_y][curr_x];
            if (metric_requirements(trailhead_score, curr_height, curr_coords, peaks_reached)) {
                continue;
            }

            for (const auto& [x_diff, y_diff] : directions) {
                const auto x = static_cast<int>(curr_x) + x_diff;
                const auto y = static_cast<int>(curr_y) + y_diff;

                if (not within_bounds(x, y) or curr_height + 1 != topographic_map[y][x]) {
                    continue;
                }

                coordinates_queue.push(Coordinates{
                    static_cast<std::size_t>(x),
                    static_cast<std::size_t>(y)
                });
            }
        }

        return trailhead_score;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto [topographic_map, trailhead_coords] = read_map_from_file(file_path);

    constexpr auto max_height = 9;
    const auto score_calculation = [](uint& curr_score, const uint height, const Coordinates& coords, CoordinatesSet& peaks_reached) {
        if (height == max_height and not peaks_reached.contains(coords)) {
            curr_score += 1;
            peaks_reached.insert(coords);
            return true;
        }
        return false;
    };
    const auto rating_calculation = [](uint& curr_rating, const uint height, const Coordinates&, CoordinatesSet&) {
        if (height == max_height) {
            curr_rating += 1;
            return true;
        }
        return false;
    };

    uint score_sum = 0;
    uint rating_sum = 0;
    for (const auto& coords : trailhead_coords) {
        score_sum += calculate_trailhead_metric(topographic_map, coords, score_calculation);
        rating_sum += calculate_trailhead_metric(topographic_map, coords, rating_calculation);
    }

    std::cout << "Score: " << score_sum << '\n';
    std::cout << "Rating: " << rating_sum << '\n';
    return 0;
}
