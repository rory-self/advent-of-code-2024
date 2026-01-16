// Libraries //
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace {
// Types //
using LocationId = int;
using LocationIdList = std::vector<LocationId>;
using LocationIdListPair = std::pair<LocationIdList, LocationIdList>;

// Prototypes //
[[nodiscard]] auto read_lists_from_file(const std::string filepath) -> LocationIdListPair;
[[nodiscard]] auto calc_list_distance(const LocationIdList l1, const LocationIdList l2) -> unsigned int;
[[nodiscard]] auto calc_similarity_score(const LocationIdList l1, const LocationIdList l2) -> unsigned int;
} // namespace

// Implementation //
auto main(const int argc, const char **argv) -> int {
  if (argc < 2) {
    throw "No input filepath provided.";
  }

  auto [list1, list2] = read_lists_from_file(argv[1]);
  std::sort(list1.begin(), list1.end());
  std::sort(list2.begin(), list2.end());

  const unsigned int total_distance = calc_list_distance(list1, list2);
  const unsigned int similarity_score = calc_similarity_score(list1, list2);
  std::cout << "Total distance: " << total_distance << " Similarity score: " << similarity_score << '\n';
  return 0;
}

namespace {
auto read_lists_from_file(const std::string filepath) -> LocationIdListPair {
  auto file = std::ifstream(filepath);

  LocationIdList list1;
  LocationIdList list2;
  for (std::string line; std::getline(file, line);) {
    const auto delim_pos = line.find_first_of(' ');

    const std::string token1 = line.substr(0, delim_pos);
    const std::string token2 = line.substr(delim_pos + 1);

    const LocationId num1 = std::stoi(token1);
    const LocationId num2 = std::stoi(token2);
    list1.push_back(num1);
    list2.push_back(num2);
  }

  return std::make_pair(list1, list2);
}

auto calc_list_distance(const LocationIdList l1, const LocationIdList l2) -> unsigned int {
  unsigned int total_distance = 0;
  for (auto it1 = l1.cbegin(), it2 = l2.cbegin(); it1 != l1.cend(); ++it1, ++it2) {
      total_distance += std::abs(*it1 - *it2);
  }

  return total_distance;
}

auto calc_similarity_score(const LocationIdList l1, const LocationIdList l2) -> unsigned int {
  std::unordered_map<LocationId, unsigned int> list2_map;
  for (const LocationId e : l2) {
    ++list2_map[e];
  }

  unsigned int similarity_score = 0;
  for (const LocationId e : l1) {
    if (list2_map.contains(e)) {
      similarity_score += e * list2_map[e];
    }
  }

  return similarity_score;
}
} // namespace

