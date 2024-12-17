#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <sstream>
#include <tuple>
#include <functional>

namespace {
    struct Direction {
        int x;
        int y;
    };
    using Directions = std::vector<Direction>;

    struct Coordinates {
        std::size_t x = 0;
        std::size_t y = 0;

        [[nodiscard]] auto operator+(const Direction& direction) const -> Coordinates {
            return {x + direction.x, y + direction.y};
        }
    };

    enum WarehousePosition {
        Wall,
        Box,
        Empty
    };
    constexpr auto warehouse_width = 50;
    constexpr auto warehouse_height = 50;
    using Warehouse = std::array<std::array<WarehousePosition, warehouse_width>, warehouse_height>;

    [[nodiscard]] auto warehouse_object_from_char(const char c) -> WarehousePosition {
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

    [[nodiscard]] auto read_warehouse_from_file(std::ifstream& file) -> std::pair<Warehouse, Coordinates> {
        Warehouse warehouse;
        Coordinates robot_location;

        std::size_t y = 0;
        for (std::string line; std::getline(file, line) and not line.empty();) {
            std::size_t x = 0;
            for (const auto c : line) {
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

    [[nodiscard]] auto read_directions_from_file(std::ifstream& file) -> Directions {
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
            for (const auto c : line) {
                read_direction(c);
            }
        }
        return directions;
    }

    [[nodiscard]] auto read_setup_from_file(const std::string& file_path) -> std::tuple<Warehouse, Coordinates, Directions> {
        auto file = std::ifstream(file_path);
        const auto [warehouse, robot_location] = read_warehouse_from_file(file);
        const auto directions = read_directions_from_file(file);

        return {warehouse, robot_location, directions};
    }

    auto process_robot_movements(Warehouse& warehouse, Coordinates& robot_location, const Directions& directions) {
        std::function<bool(WarehousePosition, const Coordinates&, const Direction&)> try_move;
        try_move = [&warehouse, &try_move](const auto object, const auto& object_pos, const auto& direction) {
            switch (object) {
                case Box: {
                    const auto new_object_pos = object_pos + direction;
                    const auto object_at_pos = warehouse[new_object_pos.y][new_object_pos.x];
                    if (try_move(object_at_pos, new_object_pos, direction)) {
                        warehouse[new_object_pos.y][new_object_pos.x] = Box;
                        warehouse[object_pos.y][object_pos.x] = Empty;

                        return true;
                    }
                    return false;
                }
                case Wall:
                    return false;
                case Empty:
                    return true;
                default:
                    throw std::runtime_error("Unknown object at position");
            }
        };

        for (const auto& direction : directions) {
            const auto new_robot_pos = robot_location + direction;
            const auto object_at_pos = warehouse[new_robot_pos.y][new_robot_pos.x];
            if (try_move(object_at_pos, new_robot_pos, direction)) {
                robot_location = new_robot_pos;
            }
        }
    }

    [[nodiscard]] auto calculate_box_gps_sum(const Warehouse& warehouse) -> uint {
        uint gps_sum = 0;
        for (std::size_t y = 0; y < warehouse_height; ++y) {
            for (std::size_t x = 0; x < warehouse_width; ++x) {
                if (warehouse[y][x] == Box) {
                    gps_sum += 100 * y + x;
                }
            }
        }
        return gps_sum;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    auto [warehouse, robot_coords, directions] = read_setup_from_file(file_path);
    process_robot_movements(warehouse, robot_coords, directions);

    const auto gps_sum = calculate_box_gps_sum(warehouse);
    std::cout << gps_sum << '\n';
    return 0;
}