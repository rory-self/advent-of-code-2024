#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>
#include <unordered_map>
#include <ranges>
#include <algorithm>
#include <unordered_set>

#include "robot.h"

namespace {
    [[nodiscard]] auto read_robots_from_file(const std::string& file_path) -> std::vector<Robot> {
        std::vector<Robot> robots;

        auto file = std::ifstream(file_path);
        for (std::string line; std::getline(file, line);) {
            const auto new_robot = Robot::from_string(line);
            robots.push_back(new_robot);
        }

        return robots;
    }

    [[nodiscard]] auto calculate_safety_factor(std::vector<Robot> robots) -> int {
        constexpr auto num_seconds = 100;
        for (auto i = 0; i < num_seconds; ++i) {
            for (auto& robot : robots) {
                robot.move();
            }
        }

        std::unordered_map<Quadrant, int> quadrant_map;
        for (const auto& robot : robots) {
            const auto robot_quadrant = robot.get_quadrant();
            if (robot_quadrant == NoQuadrant) {
                continue;
            }

            quadrant_map[robot_quadrant] += 1;
        }

        const auto quadrant_values = quadrant_map | std::views::values;
        return std::accumulate(quadrant_values.begin(), quadrant_values.end(), 1, std::multiplies());
    }

    [[nodiscard]] auto is_valid_top(
        const std::unordered_set<Coordinates, CoordinatesHash>& robots_coords,
        const Coordinates& top_robot_pos
    ) -> bool {
        constexpr auto min_tree_height = 4;

        std::vector<Coordinates> positions_to_check;
        positions_to_check.reserve(min_tree_height * 2);
        for (auto h = 1; h <= min_tree_height; ++h) {
            positions_to_check.emplace_back(-1 * h, h);
            positions_to_check.emplace_back(1 * h, h);
        }

        return std::ranges::all_of(positions_to_check, [&top_robot_pos, &robots_coords](const Coordinates& direction) {
            const auto pos_to_check = top_robot_pos + direction;
            return robots_coords.contains(pos_to_check);
        });
    }

    [[nodiscard]] auto is_christmas_tree(const std::vector<Robot>& robots) -> bool {
        std::unordered_set<Coordinates, CoordinatesHash> robot_coords;
        for (const auto& robot : robots) {
            const auto robot_position = robot.get_position();
            robot_coords.insert(robot_position);
        }

        return std::ranges::any_of(robot_coords, [robot_coords](const auto& pos) {
            return is_valid_top(robot_coords, pos);
        });
    }

    [[nodiscard]] auto time_christmas_tree_formation(std::vector<Robot>& robots) -> int {
        auto christmas_tree_time = 0;
        while (not is_christmas_tree(robots)) {
            for (auto& robot : robots) {
                robot.move();
            }

            ++christmas_tree_time;
        }

        return christmas_tree_time;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    auto robots = read_robots_from_file(file_path);

    const auto safety_factor = calculate_safety_factor(robots);
    std::cout << "The safety factor is " << safety_factor << '\n';

    const auto christmas_tree_time = time_christmas_tree_formation(robots);
    std::cout << "It took " << christmas_tree_time << " seconds to form a Christmas tree\n";
    return 0;
}