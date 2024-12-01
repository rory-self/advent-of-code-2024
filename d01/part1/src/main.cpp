#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

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

auto main() -> int {
  auto [list1, list2] = read_lists_from_file();
  std::sort(list1.begin(), list1.end());
  std::sort(list2.begin(), list2.end());

  auto total = 0;
  for (auto it1 = list1.cbegin(), it2 = list2.cbegin(); it1 != list1.cend(); ++it1, ++it2) {
    total += std::abs(*it1 - *it2);
  }

  std::cout << total << '\n';
  return 0;
}


