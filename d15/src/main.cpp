#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <tuple>
#include <unordered_set>

namespace {
    enum Direction {
        Up,
        Down,
        Left,
        Right
    };

    using Directions = std::vector<Direction>;

    struct Coordinates {
        std::size_t x = 0;
        std::size_t y = 0;

        [[nodiscard]] auto operator+(const Direction &direction) const -> Coordinates {
            std::size_t new_x_pos = x;
            std::size_t new_y_pos = y;

            switch (direction) {
                case Up:
                    new_y_pos -= 1;
                    break;
                case Down:
                    new_y_pos += 1;
                    break;
                case Left:
                    new_x_pos -= 1;
                    break;
                case Right:
                    new_x_pos += 1;
                    break;
            }
            return {new_x_pos, new_y_pos};
        }

        [[nodiscard]] auto operator==(const Coordinates &other) const noexcept -> bool = default;
    };

    struct CoordinatesHash {
        auto operator()(const Coordinates &coordinates) const noexcept -> std::size_t {
            const auto x_hash = std::hash<std::size_t>()(coordinates.x);
            const auto y_hash = std::hash<std::size_t>()(coordinates.y);

            return x_hash ^ y_hash << 1;
        }
    };

    enum Entity {
        Wall,
        Box,
        Empty,
        BoxLeft,
        BoxRight,
    };

    constexpr auto warehouse_width = 10;
    constexpr auto warehouse_height = 10;
    template<size_t N>
    using Warehouse = std::array<std::array<Entity, N>, warehouse_height>;

    struct EntityMovement {
        const Coordinates old_pos;
        const Coordinates new_pos;
        const Entity object_type;
    };

    struct MovementHash {
        auto operator()(const EntityMovement &movement) const -> std::size_t {
            const auto old_pos_hash = CoordinatesHash()(movement.old_pos);
            const auto new_pos_hash = CoordinatesHash()(movement.new_pos);
            return old_pos_hash ^ new_pos_hash << 1;
        }
    };
    using Movements = std::unordered_set<EntityMovement, MovementHash>;

    [[nodiscard]] auto warehouse_object_from_char(const char c) -> Entity {
        switch (c) {
            case '#':
                return Wall;
            case '@':
            case '.':
                return Empty;
            case 'O':
                return Box;
            default:
                throw std::runtime_error("Unknown object character");
        }
    }

    [[nodiscard]] auto read_warehouse_from_file(
        std::ifstream &file) -> std::pair<Warehouse<warehouse_width>, Coordinates> {
        Warehouse<warehouse_width> warehouse;
        Coordinates robot_location;

        std::size_t y = 0;
        for (std::string line; std::getline(file, line) and not line.empty();) {
            std::size_t x = 0;
            for (const auto c: line) {
                warehouse[y][x] = warehouse_object_from_char(c);
                if (constexpr auto robot_char = '@'; c == robot_char) {
                    robot_location = {x, y};
                }
                ++x;
            }
            ++y;
        }

        return {warehouse, robot_location};
    }

    [[nodiscard]] auto read_directions_from_file(std::ifstream &file) -> Directions {
        Directions directions;
        const auto read_direction = [&directions](const char c) {
            switch (c) {
                case '^':
                    directions.push_back(Up);
                    break;
                case 'v':
                    directions.push_back(Down);
                    break;
                case '<':
                    directions.push_back(Left);
                    break;
                case '>':
                    directions.push_back(Right);
                    break;
                default:
                    throw std::runtime_error("Unknown direction character");
            }
        };

        for (std::string line; std::getline(file, line);) {
            for (const auto c: line) {
                read_direction(c);
            }
        }
        return directions;
    }

    [[nodiscard]] auto read_setup_from_file(
        const std::string &file_path) -> std::tuple<Warehouse<warehouse_width>, Coordinates, Directions> {
        auto file = std::ifstream(file_path);
        const auto [warehouse, robot_location] = read_warehouse_from_file(file);
        const auto directions = read_directions_from_file(file);

        return {warehouse, robot_location, directions};
    }

    template<typename W>
    [[nodiscard]] auto can_move(
        W &warehouse,
        const Entity object,
        const Coordinates &object_location,
        const Direction &direction,
        Movements &movements
    ) -> bool {
        switch (object) {
            case BoxLeft:
            case BoxRight: {
                return can_move_large_box(warehouse, object, object_location, direction, movements);
            }
            case Box: {
                const auto new_object_pos = object_location + direction;
                const auto object_at_pos = warehouse[new_object_pos.y][new_object_pos.x];
                movements.insert(EntityMovement{object_location, new_object_pos, Box});

                return can_move(warehouse, object_at_pos, new_object_pos, direction, movements);
            }
            case Wall:
                return false;
            case Empty:
                return true;
            default:
                throw std::runtime_error("Unknown object at position");
        }
    }

    template<size_t N>
    [[nodiscard]] auto can_move_large_box(
        Warehouse<N> &warehouse,
        const Entity box_side,
        const Coordinates &side_location,
        const Direction &direction,
        Movements &movements
    ) -> bool {
        const auto left_location = box_side == BoxLeft ? side_location : side_location + Left;
        const auto right_location = box_side == BoxRight ? side_location : side_location + Right;

        const auto new_object_pos_left = left_location + direction;
        const auto new_object_pos_right = right_location + direction;
        const auto object_at_pos_left = warehouse[new_object_pos_left.y][new_object_pos_left.x];
        const auto object_at_pos_right = warehouse[new_object_pos_right.y][new_object_pos_right.x];

        if (direction == Left) {
            movements.insert(EntityMovement{right_location, new_object_pos_right, BoxRight});
            movements.insert(EntityMovement{left_location, new_object_pos_left, BoxLeft});
            return can_move(warehouse, object_at_pos_left, new_object_pos_left, Left, movements);
        }
        if (direction == Right) {
            movements.insert(EntityMovement{left_location, new_object_pos_left, BoxLeft});
            movements.insert(EntityMovement{right_location, new_object_pos_right, BoxRight});
            return can_move(warehouse, object_at_pos_right, new_object_pos_right, Right, movements);
        }

        const auto left_movement = EntityMovement{left_location, new_object_pos_left, BoxLeft};
        const auto right_movement = EntityMovement{right_location, new_object_pos_right, BoxRight};
        if (not movements.contains(left_movement)) {
            movements.insert(left_movement);
        }
        if (not movements.contains(right_movement)) {
            movements.insert(right_movement);
        }

        return can_move(warehouse, object_at_pos_left, new_object_pos_left, direction, movements)
               and can_move(warehouse, object_at_pos_right, new_object_pos_right, direction, movements);
    }

    template<size_t N>
    auto execute_entity_movements(Warehouse<N> &warehouse, Movements &movements) {
        for (const auto& movement : movements) {
            const auto [x, y] = movement.new_pos;
            warehouse[y][x] = movement.object_type;
        }

        for (const auto& movement : movements) {
            const auto [x, y] = movement.old_pos;
            warehouse[y][x] = Empty;
        }
    }

    template<size_t N>
    [[nodiscard]] auto process_robot_movements(
        Warehouse<N> warehouse,
        Coordinates robot_location,
        const Directions &directions
    ) -> Warehouse<N> {
        for (const auto &direction: directions) {
            Movements movements;
            const auto new_robot_pos = robot_location + direction;
            const auto object_at_pos = warehouse[new_robot_pos.y][new_robot_pos.x];
            if (can_move(warehouse, object_at_pos, new_robot_pos, direction, movements)) {
                execute_entity_movements(warehouse, movements);
                robot_location = new_robot_pos;
            }
        }

        return warehouse;
    }

    template<size_t N>
    [[nodiscard]] auto calculate_box_gps_sum(const Warehouse<N> &warehouse, const std::size_t width) -> uint {
        uint gps_sum = 0;
        for (std::size_t y = 0; y < warehouse_height; ++y) {
            for (std::size_t x = 0; x < width; ++x) {
                const auto object = warehouse[y][x];
                if (object == Box or object == BoxLeft) {
                    gps_sum += 100 * y + x;
                }
            }
        }
        return gps_sum;
    }

    [[nodiscard]] auto build_wide_warehouse_from_warehouse(
        const Warehouse<warehouse_width> &warehouse,
        const Coordinates &robot_start
    ) -> std::pair<Warehouse<warehouse_width * 2>, Coordinates> {
        Warehouse<warehouse_width * 2> wide_warehouse;

        for (std::size_t y = 0; y < warehouse_height; ++y) {
            for (std::size_t x = 0; x < warehouse_width; ++x) {
                const auto object = warehouse[y][x];

                const auto left_x_pos = x * 2;
                wide_warehouse[y][left_x_pos] = object == Box ? BoxLeft : object;
                wide_warehouse[y][left_x_pos + 1] = object == Box ? BoxRight : object;
            }
        }

        const auto wide_robot_start = Coordinates{
            robot_start.x * 2,
            robot_start.y,
        };

        return {wide_warehouse, wide_robot_start};
    }
}

auto main() -> int {
    const auto file_path = std::string{"test.txt"};
    const auto [warehouse, robot_start, directions] = read_setup_from_file(file_path);

    const auto moved_warehouse = process_robot_movements(warehouse, robot_start, directions);
    const auto gps_sum = calculate_box_gps_sum(moved_warehouse, warehouse_width);
    std::cout << "GPS sum of normal warehouse: " << gps_sum << '\n';

    const auto [wide_warehouse, wide_robot_start] = build_wide_warehouse_from_warehouse(warehouse, robot_start);
    const auto wide_moved_warehouse = process_robot_movements(wide_warehouse, wide_robot_start, directions);
    const auto wide_gps_sum = calculate_box_gps_sum(wide_moved_warehouse, warehouse_width * 2);
    std::cout << "GPS sum of wide warehouse: " << wide_gps_sum << '\n';

    return 0;
}
