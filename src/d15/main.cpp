#include <array>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

namespace {
enum Entity { Wall, Box, Empty, BoxLeft, BoxRight };
constexpr char robot_char = '@';

constexpr std::size_t warehouse_height = 50;

template <std::size_t N>
using Warehouse = std::array<std::array<Entity, N>, warehouse_height>;

enum Direction { Up, Down, Left, Right };
using Directions = std::vector<Direction>;

using uint = unsigned int;
struct Coordinates {
  uint x;
  uint y;

  [[nodiscard]] auto operator+(const Direction direction) const -> Coordinates {
    switch (direction) {
    case Up:
      return {x, y - 1};
    case Down:
      return {x, y + 1};
    case Left:
      return {x - 1, y};
    case Right:
      return {x + 1, y};
    default:
      throw std::logic_error{"Invalid direction"};
    }
  }

  [[nodiscard]] auto operator==(const Coordinates &rhs) const noexcept
      -> bool = default;
  auto operator=(const Coordinates &rhs) -> Coordinates & = default;
};

struct CoordinatesHash {
  [[nodiscard]] auto operator()(const Coordinates &coords) const
      -> std::size_t {
    const auto h1 = std::hash<uint>{}(coords.x);
    const auto h2 = std::hash<uint>{}(coords.y);

    return h1 ^ h2 << 1;
  }
};

constexpr std::size_t warehouse_width = 50;
struct WarehouseSetup {
  const Warehouse<warehouse_width> warehouse;
  const Coordinates robot_start;
  const Directions robot_directions;
};

struct EntityMovement {
  const Coordinates old_pos;
  const Coordinates new_pos;
  const Entity entity_type;
};

// Prototypes
[[nodiscard]] auto read_setup_from_file(const std::string &file_path)
    -> WarehouseSetup;
[[nodiscard]] auto read_warehouse_from_file(std::ifstream &fp)
    -> std::pair<Warehouse<warehouse_width>, Coordinates>;
[[nodiscard]] auto entity_from_char(const char c) -> Entity;
[[nodiscard]] auto read_directions_from_file(std::ifstream &fp) -> Directions;

template <std::size_t N>
[[nodiscard]] auto process_warehouse(const Warehouse<N> &warehouse,
                                     const Coordinates &robot_start,
                                     const Directions &directions) -> uint;

template <std::size_t N>
[[nodiscard]] auto process_robot_movements(const Warehouse<N> &warehouse,
                                           const Coordinates &robot_start,
                                           const Directions &directions)
    -> Warehouse<N>;

template <std::size_t N>
[[nodiscard]] auto
check_entity_movement(Warehouse<N> &warehouse, const Coordinates &dest,
                      const Direction direction,
                      std::stack<EntityMovement> &entity_movements) -> bool;

template <std::size_t N>
void move_entities(Warehouse<N> &warehouse,
                   std::stack<EntityMovement> entity_movements);
template <std::size_t N>
[[nodiscard]] auto calculate_box_gps_sum(const Warehouse<N> &warehouse) -> uint;

template <std::size_t N>
[[nodiscard]] auto create_wide_warehouse(const Warehouse<N> &warehouse,
                                         const Coordinates &robot_start)
    -> std::pair<Warehouse<N * 2>, Coordinates>;
} // namespace

auto main() -> int {
  const auto file_path = std::string{"input.txt"};
  const auto [warehouse, robot_start, directions] =
      read_setup_from_file(file_path);

  const uint small_gps_sum =
      process_warehouse(warehouse, robot_start, directions);
  std::cout << "Small warehouse box gps sum: " << small_gps_sum << '\n';

  const auto [wide_warehouse, wide_robot_start] =
      create_wide_warehouse(warehouse, robot_start);
  const uint wide_gps_sum =
      process_warehouse(wide_warehouse, wide_robot_start, directions);
  std::cout << "Wide warehouse box gps sum: " << wide_gps_sum << '\n';

  return 0;
}

namespace {
template <std::size_t N>
[[nodiscard]] auto process_warehouse(const Warehouse<N> &warehouse,
                                     const Coordinates &robot_start,
                                     const Directions &directions) -> uint {
  const auto moved_warehouse =
      process_robot_movements(warehouse, robot_start, directions);
  return calculate_box_gps_sum(moved_warehouse);
}

[[nodiscard]] auto read_setup_from_file(const std::string &file_path)
    -> WarehouseSetup {
  auto fp = std::ifstream{file_path};
  const auto [warehouse, robot_location] = read_warehouse_from_file(fp);
  const auto directions = read_directions_from_file(fp);

  return {warehouse, robot_location, directions};
}

[[nodiscard]] auto read_warehouse_from_file(std::ifstream &fp)
    -> std::pair<Warehouse<warehouse_width>, Coordinates> {
  Warehouse<warehouse_width> warehouse;
  uint robot_x, robot_y;

  std::size_t y = 0;
  for (std::string line; std::getline(fp, line) and not line.empty(); ++y) {
    std::size_t x = 0;
    for (const char c : line) {
      warehouse[y][x] = entity_from_char(c);

      if (c == robot_char) {
        robot_x = x;
        robot_y = y;
      }

      ++x;
    }
  }

  const auto robot_start = Coordinates{robot_x, robot_y};
  return {warehouse, robot_start};
}

[[nodiscard]] auto entity_from_char(const char c) -> Entity {
  switch (c) {
  case '#':
    return Wall;
  case robot_char:
  case '.':
    return Empty;
  case 'O':
    return Box;
  default:
    throw std::runtime_error("Unknown entity character");
  }
}

[[nodiscard]] auto read_directions_from_file(std::ifstream &fp) -> Directions {
  Directions directions;

  const auto read_direction = [&directions](const char c) {
    switch (c) {
    case '^':
      directions.push_back(Up);
      break;
    case 'v':
      directions.push_back(Down);
      break;
    case '>':
      directions.push_back(Right);
      break;
    case '<':
      directions.push_back(Left);
      break;
    default:
      throw std::runtime_error("Unknown direction character");
    }
  };

  for (std::string line; std::getline(fp, line);) {
    for (const auto c : line) {
      read_direction(c);
    }
  }

  return directions;
}

template <std::size_t N>
[[nodiscard]] auto process_robot_movements(const Warehouse<N> &start_warehouse,
                                           const Coordinates &robot_start,
                                           const Directions &directions)
    -> Warehouse<N> {
  Warehouse<N> warehouse = start_warehouse;
  Coordinates robot_location = robot_start;
  for (const auto &direction : directions) {
    std::stack<EntityMovement> entity_movements = {};
    const Coordinates new_robot_pos = robot_location + direction;
    if (check_entity_movement(warehouse, new_robot_pos, direction,
                              entity_movements)) {
      move_entities(warehouse, entity_movements);
      robot_location = new_robot_pos;
    }
  }

  return warehouse;
}

template <std::size_t N>
[[nodiscard]] auto
check_entity_movement(Warehouse<N> &warehouse, const Coordinates &dest,
                      const Direction direction,
                      std::stack<EntityMovement> &entity_movements) -> bool {
  const auto simple_push = [&entity_movements,
                            direction](const Coordinates original_pos,
                                       const Entity entity) {
    const auto box_dest = original_pos + direction;
    const auto box_movement = EntityMovement(original_pos, box_dest, entity);
    entity_movements.push(box_movement);
    return box_dest;
  };

  const auto check_large_box_movement =
      [direction, &entity_movements, &warehouse](
          const Coordinates &left_dest, const Coordinates &right_dest) {
        switch (direction) {
        case Up:
        case Down:
          return check_entity_movement(warehouse, left_dest, direction,
                                       entity_movements) and
                 check_entity_movement(warehouse, right_dest, direction,
                                       entity_movements);
        case Right:
          return check_entity_movement(warehouse, right_dest, direction,
                                       entity_movements);
        case Left:
          return check_entity_movement(warehouse, left_dest, direction,
                                       entity_movements);
        default:
          throw std::logic_error("Invalid direction");
        };
      };

  const auto &[dest_x, dest_y] = dest;
  const Entity object_at_dest = warehouse[dest_y][dest_x];
  switch (object_at_dest) {
  case Box: {
    const Coordinates &box_dest = simple_push(dest, Box);
    return check_entity_movement(warehouse, box_dest, direction,
                                 entity_movements);
  };
  case BoxLeft: {
    const auto right_side_pos = dest + Right;
    const Coordinates left_dest = simple_push(dest, BoxLeft);
    const Coordinates right_dest = simple_push(right_side_pos, BoxRight);
    return check_large_box_movement(left_dest, right_dest);
  };
  case BoxRight: {
    const auto left_side_pos = dest + Left;
    const Coordinates right_dest = simple_push(dest, BoxRight);
    const Coordinates left_dest = simple_push(left_side_pos, BoxLeft);
    return check_large_box_movement(left_dest, right_dest);
  };
  case Wall:
    return false;
  case Empty:
    return true;
  default:
    throw std::logic_error("Unknown entity");
  }
}

template <std::size_t N>
void move_entities(Warehouse<N> &warehouse,
                   std::stack<EntityMovement> entity_movements) {
  std::unordered_set<Coordinates, CoordinatesHash> visited_dests;
  while (not entity_movements.empty()) {
    const auto [old_pos, new_pos, entity_type] = entity_movements.top();
    entity_movements.pop();
    if (visited_dests.contains(new_pos)) {
      continue;
    }

    visited_dests.insert(new_pos);
    warehouse[old_pos.y][old_pos.x] = Empty;
    warehouse[new_pos.y][new_pos.x] = entity_type;
  }
}

template <size_t N>
[[nodiscard]] auto calculate_box_gps_sum(const Warehouse<N> &warehouse)
    -> uint {
  uint gps_sum = 0;
  for (std::size_t y = 0; y < warehouse_height; ++y) {
    for (std::size_t x = 0; x < N; ++x) {
      const auto object = warehouse[y][x];
      if (object == Box or object == BoxLeft) {
        gps_sum += 100 * y + x;
      }
    }
  }

  return gps_sum;
}

template <std::size_t N>
[[nodiscard]] auto create_wide_warehouse(const Warehouse<N> &warehouse,
                                         const Coordinates &robot_start)
    -> std::pair<Warehouse<N * 2>, Coordinates> {
  Warehouse<N * 2> wide_warehouse;

  for (std::size_t y = 0; y < warehouse_height; ++y) {
    for (std::size_t x = 0; x < N; ++x) {
      const Entity small_entity = warehouse[y][x];

      const uint wide_x = x * 2;
      if (small_entity == Box) {
        wide_warehouse[y][wide_x] = BoxLeft;
        wide_warehouse[y][wide_x + 1] = BoxRight;
        continue;
      }

      wide_warehouse[y][wide_x] = small_entity;
      wide_warehouse[y][wide_x + 1] = small_entity;
    }
  }

  const auto wide_robot_start = Coordinates{robot_start.x * 2, robot_start.y};
  return {wide_warehouse, wide_robot_start};
}
} // namespace
