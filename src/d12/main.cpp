#include <array>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

namespace {
using uint = unsigned int;
struct Region {
  const uint perimeter;
  const uint area;
  const uint sides;
};

using Direction = std::pair<int, int>;

struct DirectionHash {
  [[nodiscard]] auto operator()(const Direction &direction) const noexcept
      -> std::size_t {
    const auto h1 = std::hash<int>()(direction.first);
    const auto h2 = std::hash<int>()(direction.second);

    return h1 ^ h2 << 1;
  }
};

struct Coordinates {
  const int x;
  const int y;

  Coordinates(const int x, const int y) : x(x), y(y) {}
  Coordinates(const std::size_t &x, const std::size_t &y) : x(x), y(y) {}

  [[nodiscard]] auto operator==(const Coordinates &rhs) const -> bool = default;
  [[nodiscard]] auto operator+(const Direction &direction) const noexcept
      -> Coordinates {
    const auto new_x = this->x + direction.first;
    const auto new_y = this->y + direction.second;

    return {new_x, new_y};
  }
};

using DirectionSet = std::unordered_set<Direction, DirectionHash>;

constexpr Direction directions[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
constexpr Direction adjacent_directions[4][2] = {
    {{1, 0}, {0, 1}}, {{0, 1}, {-1, 0}}, {{-1, 0}, {0, -1}}, {{0, -1}, {1, 0}}};

struct Plot {
  char plant_type;
  bool visited = false;
};

constexpr std::size_t plot_width = 140;
constexpr std::size_t plot_height = 140;
using PlotArray = std::array<std::array<Plot, plot_width>, plot_height>;

// Prototypes
[[nodiscard]] auto read_plots_from_file(const std::string &rel_file_path)
    -> PlotArray;
[[nodiscard]] auto plot_array_to_regions(PlotArray &plot_array)
    -> std::vector<Region>;
[[nodiscard]] auto new_plot_region(PlotArray &plot_array,
                                   const Coordinates &start_coords,
                                   const char plant_type) -> Region;
} // namespace

auto main() -> int {
  const std::string file_path = "input.txt";
  PlotArray plot_array = read_plots_from_file(file_path);

  const std::vector<Region> regions = plot_array_to_regions(plot_array);

  uint fencing_price = 0;
  uint discounted_fence_price = 0;
  for (const auto &[perimeter, area, sides] : regions) {
    fencing_price += perimeter * area;
    discounted_fence_price += area * sides;
  }
  std::cout << "Fence price without discount: " << fencing_price << '\n';
  std::cout << "Fence price with discount: " << discounted_fence_price << '\n';

  return 0;
}

namespace {
[[nodiscard]] auto read_plots_from_file(const std::string &rel_file_path)
    -> PlotArray {
  PlotArray plot_array;
  auto file = std::ifstream(rel_file_path);

  std::size_t row = 0;
  for (std::string line; std::getline(file, line); ++row) {
    std::size_t column = 0;
    for (const auto c : line) {
      plot_array[row][column] = Plot{c};
      ++column;
    }
  }

  return plot_array;
}

[[nodiscard]] auto plot_array_to_regions(PlotArray &plot_array)
    -> std::vector<Region> {
  std::vector<Region> regions;

  // Find first plant of given type and group bordering plants into region
  for (std::size_t row = 0; row < plot_width; ++row) {
    for (std::size_t col = 0; col < plot_height; ++col) {
      const auto curr_coords = Coordinates{col, row};
      auto &[plant_type, visited] = plot_array[row][col];
      if (visited) {
        continue;
      }

      visited = true;
      const Region new_region =
          new_plot_region(plot_array, curr_coords, plant_type);
      regions.push_back(new_region);
    }
  }

  return regions;
}

[[nodiscard]] auto new_plot_region(PlotArray &plot_array,
                                   const Coordinates &start_coords,
                                   const char plant_type) -> Region {
  uint area = 0;
  uint perimeter = 0;
  uint sides = 0;

  const auto is_empty_coordinate = [&plot_array,
                                    &plant_type](const Coordinates &coords) {
    const auto [x, y] = coords;
    if (x < 0 or x >= plot_width or y < 0 or y >= plot_height) {
      return true;
    }

    auto &[adjacent_plant_type, visited] = plot_array[y][x];
    return adjacent_plant_type != plant_type;
  };

  // Perform BFS to find region
  const auto [start_x, start_y] = start_coords;
  plot_array[start_y][start_x].visited = true;
  auto plot_queue = std::queue<Coordinates>{{start_coords}};
  while (not plot_queue.empty()) {
    ++area;

    const auto curr_coords = plot_queue.front();
    plot_queue.pop();

    // Check perimeter and queue next plots
    const auto [curr_x, curr_y] = curr_coords;
    for (const auto &direction : directions) {
      const auto new_coords = curr_coords + direction;
      if (is_empty_coordinate(new_coords)) {
        ++perimeter;
        continue;
      }

      const auto [new_x, new_y] = new_coords;
      auto &visited = plot_array[new_y][new_x].visited;
      if (visited) {
        continue;
      }
      visited = true;
      plot_queue.push(new_coords);
    }

    // Check for corners (and hence edges)
    for (const auto &direction_pair : adjacent_directions) {
      const auto &[direction1, direction2] = direction_pair;
      const auto coord1 = curr_coords + direction1;
      const auto coord2 = curr_coords + direction2;

      const auto coord1_empty = is_empty_coordinate(coord1);
      const auto coord2_empty = is_empty_coordinate(coord2);
      if (coord1_empty and coord2_empty) {
        ++sides;
        continue;
      }

      const auto corner_coord = curr_coords + direction1 + direction2;
      if (not coord1_empty and not coord2_empty and
          is_empty_coordinate(corner_coord)) {
        ++sides;
      }
    }
  }

  return {perimeter, area, sides};
}
} // namespace
