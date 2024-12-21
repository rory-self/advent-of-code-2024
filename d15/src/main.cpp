#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <tuple>

namespace {
    struct Direction {
        int x;
        int y;

        [[nodiscard]] auto operator==(const Direction &rhs) const noexcept -> bool = default;
    };
    using Directions = std::vector<Direction>;

    struct Coordinates {
        std::size_t x = 0;
        std::size_t y = 0;

        [[nodiscard]] auto operator+(const Direction &direction) const -> Coordinates {
            return {x + direction.x, y + direction.y};
        }
    };

    enum Entity {
        Wall,
        Box,
        Empty,
        BoxLeft,
        BoxRight,
    };

    constexpr auto warehouse_width = 50;
    constexpr auto warehouse_height = 50;
    using Warehouse = std::array<std::array<Entity, warehouse_width>, warehouse_height>;
    using WideWarehouse = std::array<std::array<Entity, warehouse_width * 2>, warehouse_height>;

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

    [[nodiscard]] auto read_warehouse_from_file(std::ifstream &file) -> std::pair<Warehouse, Coordinates> {
        Warehouse warehouse;
        Coordinates robot_location;

        std::size_t y = 0;
        for (std::string line; std::getline(file, line) and not line.empty();) {
            std::size_t x = 0;
            for (const auto c: line) {
                warehouse[y][x] = warehouse_object_from_char(c);
                if (constexpr auto robot_char = '@'; c == robot_char) {
                    robot_location = Coordinates{x, y};
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
                    directions.emplace_back(0, -1);
                    break;
                case 'v':
                    directions.emplace_back(0, 1);
                    break;
                case '<':
                    directions.emplace_back(-1, 0);
                    break;
                case '>':
                    directions.emplace_back(1, 0);
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
        const std::string &file_path) -> std::tuple<Warehouse, Coordinates, Directions> {
        auto file = std::ifstream(file_path);
        const auto [warehouse, robot_location] = read_warehouse_from_file(file);
        const auto directions = read_directions_from_file(file);

        return {warehouse, robot_location, directions};
    }

    template<typename W>
    auto can_move(
        W &warehouse,
        const Entity object,
        const Coordinates &object_location,
        const Direction &direction,
        const bool execute_move
    ) -> bool {
        switch (object) {
            case BoxLeft:
            case BoxRight: {
                return can_move_large_box(warehouse, object, object_location, direction, execute_move);
            }
            case Box: {
                const auto new_object_pos = object_location + direction;
                const auto object_at_pos = warehouse[new_object_pos.y][new_object_pos.x];
                const bool movable = can_move(warehouse, object_at_pos, new_object_pos, direction, execute_move);
                if (execute_move and movable) {
                    warehouse[object_location.y][object_location.x] = Empty;
                    warehouse[new_object_pos.y][new_object_pos.x] = object;
                }

                return movable;
            }
            case Wall:
                return false;
            case Empty:
                return true;
            default:
                throw std::runtime_error("Unknown object at position");
        }
    }

    template<typename W>
    auto can_move_large_box(
        W &warehouse,
        const Entity box_side,
        const Coordinates &side_location,
        const Direction &direction,
        const bool execute_move
    ) -> bool {
        constexpr auto left = Direction{-1, 0};
        constexpr auto right = Direction{1, 0};

        const auto left_location = box_side == BoxLeft
                                       ? side_location
                                       : side_location + left;
        const auto right_location = box_side == BoxRight
                                        ? side_location
                                        : side_location + right;

        const auto new_object_pos_left = left_location + direction;
        const auto new_object_pos_right = right_location + direction;
        const auto object_at_pos_left = warehouse[new_object_pos_left.y][new_object_pos_left.x];
        const auto object_at_pos_right = warehouse[new_object_pos_right.y][new_object_pos_right.x];

        bool moveable = false;
        if (direction.y != 0) {
            moveable = can_move(warehouse, object_at_pos_left, new_object_pos_left, direction, execute_move)
                       and can_move(warehouse, object_at_pos_right, new_object_pos_right, direction, execute_move);
        } else if (direction == left) {
            moveable = can_move(warehouse, object_at_pos_left, new_object_pos_left, left, execute_move);
        } else {
            moveable = can_move(warehouse, object_at_pos_right, new_object_pos_right, right, execute_move);
        }

        if (moveable and execute_move) {
            warehouse[left_location.y][left_location.x] = Empty;
            warehouse[right_location.y][right_location.x] = Empty;
            warehouse[new_object_pos_left.y][new_object_pos_left.x] = BoxLeft;
            warehouse[new_object_pos_right.y][new_object_pos_right.x] = BoxRight;
        }
        return moveable;
    }

    template<typename W>
    [[nodiscard]] auto process_robot_movements(W warehouse, Coordinates robot_location,
                                               const Directions &directions) -> W {
        for (const auto &direction: directions) {
            const auto new_robot_pos = robot_location + direction;
            const auto object_at_pos = warehouse[new_robot_pos.y][new_robot_pos.x];
            if (can_move(warehouse, object_at_pos, new_robot_pos, direction, false)) {
                can_move(warehouse, object_at_pos, new_robot_pos, direction, true);
                robot_location = new_robot_pos;
            }
        }

        return warehouse;
    }

    template<typename W>
    [[nodiscard]] auto calculate_box_gps_sum(const W &warehouse, const std::size_t width) -> uint {
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

    [[nodiscard]] auto build_wide_warehouse_from_warehouse(const Warehouse &warehouse) -> WideWarehouse {
        WideWarehouse wide_warehouse;

        for (std::size_t y = 0; y < warehouse_height; ++y) {
            for (std::size_t x = 0; x < warehouse_width; ++x) {
                const auto object = warehouse[y][x];

                wide_warehouse[y][x * 2] = object == Box ? BoxLeft : object;
                wide_warehouse[y][x * 2 + 1] = object == Box ? BoxRight : object;
            }
        }

        return wide_warehouse;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto [warehouse, robot_start, directions] = read_setup_from_file(file_path);
    const auto wide_warehouse = build_wide_warehouse_from_warehouse(warehouse);

    const auto moved_warehouse = process_robot_movements(warehouse, robot_start, directions);
    const auto gps_sum = calculate_box_gps_sum(moved_warehouse, warehouse_width);
    std::cout << "GPS sum of normal warehouse: " << gps_sum << '\n';

    const auto wide_moved_warehouse = process_robot_movements(wide_warehouse, robot_start, directions);
    const auto wide_gps_sum = calculate_box_gps_sum(wide_moved_warehouse, warehouse_width * 2);
    std::cout << "GPS sum of wide warehouse: " << wide_gps_sum << '\n';

    return 0;
}
