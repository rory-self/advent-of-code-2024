#include <iostream>
#include <array>
#include <vector>
#include <fstream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

namespace {
    constexpr std::size_t plot_width = 140;
    constexpr std::size_t plot_height = 140;
    struct Plot {
        char plant_type;
        bool visited;
    };
    using PlotMap = std::array<std::array<Plot, plot_width>, plot_height>;

    struct Region {
        std::size_t perimeter = 0;
        std::size_t area = 0;
        std::size_t sides = 0;
    };

    struct Coordinates {
        int x;
        int y;

        [[nodiscard]] auto operator==(const Coordinates& other) const noexcept -> bool = default;
    };
    struct CoordinatesHasher {
        [[nodiscard]] auto operator()(const Coordinates& coords) const -> std::size_t {
            const auto h1 = std::hash<int>()(coords.x);
            const auto h2 = std::hash<int>()(coords.y);

            // Combine field hash values
            return h1 ^ h2 << 1;
        }
    };

    using Direction = std::pair<int, int>;
    constexpr Direction directions[4] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
    struct DirectionHasher {
        [[nodiscard]] auto operator()(const Direction& direction) const -> std::size_t {
            const auto h1 = std::hash<int>()(direction.first);
            const auto h2 = std::hash<int>()(direction.second);

            return h1 ^ h2 << 1;
        }
    };

    using CoordinateDirectionMap = std::unordered_map<Coordinates, std::unordered_set<Direction, DirectionHasher>, CoordinatesHasher>;

    [[nodiscard]] auto read_plot_map_from_file(const std::string& file_path) -> PlotMap {
        PlotMap plot_map;
        auto file = std::ifstream(file_path);

        std::size_t row = 0;
        for (std::string line; std::getline(file, line); ++row) {
            std::size_t column = 0;
            for (const auto c : line) {
                plot_map[row][column] = Plot{c, false};
                ++column;
            }
        }

        return plot_map;
    }

    [[nodiscard]] auto is_new_side(
        CoordinateDirectionMap& adjacent_plots,
        const Coordinates& coords,
        const Direction& base_direction
    ) -> bool {
        const auto is_existing_adjacency = [adjacent_plots, coords, base_direction](const Direction& direction) -> bool {
            const auto [x_diff, y_diff] = direction;
            const auto [base_x_diff, base_y_diff] = base_direction;

            // Check direction is perpendicular to the base direction
            if ((base_x_diff != 0 and x_diff != 0) or (y_diff != 0 and base_y_diff != 0)) {
                return false;
            }

            const auto new_x = coords.x + x_diff;
            const auto new_y = coords.y + y_diff;

            if (const auto coords_to_check = Coordinates{new_x, new_y}; adjacent_plots.contains(coords_to_check)) {
                const auto& adjacent_plot_directions = adjacent_plots.at(coords_to_check);
                return adjacent_plot_directions.contains(base_direction);
            }

            return false;
        };

        if (adjacent_plots.contains(coords)) {
            auto& adjacent_plot_directions = adjacent_plots.at(coords);
            adjacent_plot_directions.insert(base_direction);
        } else {
            const auto direction_set = std::unordered_set<Direction, DirectionHasher>{{base_direction}};
            adjacent_plots[coords] = direction_set;
        }

        return std::ranges::none_of(directions, is_existing_adjacency);
    }

    [[nodiscard]] auto create_plot_region(
        PlotMap& plot_map,
        const Coordinates& start_coords,
        const char plant_type
    ) -> Region {
        Region new_region;
        CoordinateDirectionMap adjacent_plots;
        auto plot_queue = std::queue<Coordinates>{{start_coords}};

        const auto in_bounds = [](const int x, const int y) -> bool {
            return x >= 0 and y >= 0 and x < plot_width and y < plot_height;
        };

        const auto process_perimeter = [&new_region, &adjacent_plots](const Coordinates& coords, const Direction& direction) {
            new_region.perimeter++;

            if (is_new_side(adjacent_plots, coords, direction)) {
                new_region.sides++;
            }
        };

        while (not plot_queue.empty()) {
            new_region.area++;

            const auto curr_coords = plot_queue.front();
            plot_queue.pop();

            const auto [curr_x, curr_y] = curr_coords;

            const auto signed_x = static_cast<int>(curr_x);
            const auto signed_y = static_cast<int>(curr_y);

            for (const auto& direction : directions) {
                const auto [x_diff, y_diff] = direction;
                const auto new_x = signed_x + x_diff;
                const auto new_y = signed_y + y_diff;
                const auto new_coords = Coordinates{new_x, new_y};

                if (not in_bounds(new_x, new_y)) {
                    process_perimeter(new_coords, direction);
                    continue;
                }
                auto& [adjacent_plant_type, visited] = plot_map[new_y][new_x];
                if (adjacent_plant_type != plant_type) {
                    process_perimeter(new_coords, direction);
                    continue;
                }
                if (visited) {
                    continue;
                }

                visited = true;
                plot_queue.push(new_coords);
            }
        }

        return new_region;
    }

    [[nodiscard]] auto get_regions_from_plot_map(PlotMap& plot_map) -> std::vector<Region> {
        std::vector<Region> regions;
        for (std::size_t row = 0; row < plot_width; ++row) {
            for (std::size_t column = 0; column < plot_width; ++column) {
                const auto curr_coords = Coordinates{
                    static_cast<int>(column),
                    static_cast<int>(row)
                };
                auto& [plant_type, visited] = plot_map[row][column];
                if (visited) {
                    continue;
                }
                visited = true;

                const auto new_region = create_plot_region(plot_map, curr_coords, plant_type);
                regions.push_back(new_region);
            }
        }

        return regions;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    auto plot_map = read_plot_map_from_file(file_path);

    const auto regions = get_regions_from_plot_map(plot_map);
    std::size_t fencing_price = 0;
    std::size_t discounted_fencing_price = 0;
    for (const auto& [perimeter, area, sides] : regions) {
        fencing_price += perimeter * area;
        discounted_fencing_price += sides * area;
    }

    std::cout << "Fence price without discount: " << fencing_price << '\n';
    std::cout << "Discounted Fence price: " << discounted_fencing_price << '\n';
    return 0;
}