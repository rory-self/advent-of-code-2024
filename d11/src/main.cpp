#include <iostream>
#include <vector>
#include <fstream>

namespace {
    [[nodiscard]] auto read_stones_from_file(const std::string& file_path) -> std::vector<unsigned long> {
        auto file = std::ifstream(file_path);

        std::vector<unsigned long> stones;
        constexpr auto stone_delimiter = ' ';
        for (std::string stone_str; std::getline(file, stone_str, stone_delimiter);) {
            const auto stone = std::stoul(stone_str);
            stones.push_back(stone);
        }

        return stones;
    }

    auto blink(std::vector<unsigned long>& stones) -> void {
        for (auto it = stones.begin(); it != stones.end(); ++it) {
            if (*it == 0) {
                *it = 1;
            } else if (const auto stone_str = std::to_string(*it); stone_str.size() % 2 == 0) {
                const auto center_pos = stone_str.size() / 2;
                const auto first_half = stone_str.substr(0, center_pos);
                const auto second_half = stone_str.substr(center_pos);

                *it = std::stoul(second_half);
                const auto new_stone = std::stoul(first_half);

                const auto new_stone_it = stones.insert(it, new_stone);
                it = std::next(new_stone_it);
            } else {
                *it *= 2024;
            }
        }
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto stones = read_stones_from_file(file_path);

    auto stones_copy = stones;
    for (auto i = 0; i < 25; ++i) {
        blink(stones_copy);
    }
    std::cout << "You have " << stones_copy.size() << " stones after blinking 25 times" << '\n';

    return 0;
}