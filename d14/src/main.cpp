#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>
#include <unordered_map>
#include <ranges>

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
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    auto robots = read_robots_from_file(file_path);

    constexpr auto num_seconds = 100;
    for (auto i = 0; i < num_seconds; ++i) {
        for (auto& robot : robots) {
            robot.move();
        }
    }

    auto quadrant_map = std::unordered_map<Quadrant, int>{};
    for (const auto& robot : robots) {
        const auto robot_quadrant = robot.get_quadrant();
        if (robot_quadrant == NoQuadrant) {
            continue;
        }

        quadrant_map[robot_quadrant] += 1;
    }

    const auto quadrant_values = quadrant_map | std::views::values;
    const auto safety_factor = std::accumulate(quadrant_values.begin(), quadrant_values.end(), 1, std::multiplies());

    std::cout << "The safety factor is " << safety_factor << '\n';
    return 0;
}