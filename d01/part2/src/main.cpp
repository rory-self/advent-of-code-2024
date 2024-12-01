#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>

namespace {
    auto read_lists_from_file() -> std::pair<std::vector<int>, std::vector<int>> {
        const auto filename = "input.txt";
        auto file = std::ifstream(filename);

        auto list1 = std::vector<int> {};
        auto list2 = std::vector<int> {};
        for (auto line = std::string{}; std::getline(file, line);) {
            auto delim_pos = line.find_first_of(' ');

            auto token1 = line.substr(0, delim_pos);
            auto token2 = line.substr(delim_pos + 1);

            auto num1 = std::stoi(token1);
            auto num2 = std::stoi(token2);
            list1.push_back(num1);
            list2.push_back(num2);
        }

        return std::make_pair(list1, list2);
    }
}

int main() {
    auto [list1, list2] = read_lists_from_file();

    auto list2_map = std::unordered_map<int, int> {};
    for (const auto& e : list2) {
        list2_map[e]++;
    }

    auto score = 0;
    for (const auto& e : list1) {
        if (list2_map.find(e) == list2_map.end()) {
            continue;
        }

        score += e * list2_map[e];
    }

    std::cout << score << '\n';
    return 0;
}