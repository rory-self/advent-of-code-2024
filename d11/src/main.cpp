#include <cmath>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace {
using uint = unsigned int;
using ulong = unsigned long;

struct StoneBlinkHash {
  [[nodiscard]] auto
  operator()(const std::pair<ulong, uint> &blink_pair) const noexcept
      -> std::size_t {
    const auto h1 = std::hash<ulong>{}(blink_pair.first);
    const auto h2 = std::hash<uint>{}(blink_pair.second);

    return h1 ^ h2 << 1;
  }
};

using StoneBlinkMap =
    std::unordered_map<std::pair<ulong, uint>, ulong, StoneBlinkHash>;
using Stones = std::vector<ulong>;

// Prototypes //
[[nodiscard]] auto read_stones_from_file(const std::string &file_path)
    -> Stones;
auto n_blinks_on_stone_collection(const Stones &stones, const uint n,
                                  StoneBlinkMap &blink_map) -> void;
[[nodiscard]] auto get_num_stones(const ulong &stone,
                                  const uint remaining_blinks,
                                  StoneBlinkMap &blink_map) -> ulong;
[[nodiscard]] auto blink_stone(const ulong &stone) -> Stones;
[[nodiscard]] auto get_num_digits(ulong stone) noexcept -> uint;
[[nodiscard]] auto split_stone(const ulong &stone, const uint len)
    -> std::pair<ulong, ulong>;
} // namespace

auto main() -> int {
  StoneBlinkMap blink_map;

  const auto file_path = std::string{"input.txt"};
  const auto stones = read_stones_from_file(file_path);

  n_blinks_on_stone_collection(stones, 25, blink_map);
  n_blinks_on_stone_collection(stones, 75, blink_map);
}

// Function implementations //
namespace {
[[nodiscard]] auto read_stones_from_file(const std::string &file_path)
    -> Stones {
  auto file = std::ifstream(file_path);

  Stones stones;
  constexpr auto stone_delimiter = ' ';
  for (std::string stone_str; std::getline(file, stone_str, stone_delimiter);) {
    const auto stone = std::stoul(stone_str);
    stones.push_back(stone);
  }

  return stones;
}

auto n_blinks_on_stone_collection(const Stones &stones, const uint n,
                                  StoneBlinkMap &blink_map) -> void {
  ulong num_stones = 0;
  for (const ulong &stone : stones) {
    num_stones += get_num_stones(stone, n, blink_map);
  }

  std::cout << "You have " << num_stones << " stones after " << n
            << " blinks\n";
}

[[nodiscard]] auto get_num_stones(const ulong &stone,
                                  const uint remaining_blinks,
                                  StoneBlinkMap &blink_map) -> ulong {
  if (remaining_blinks == 0) {
    return 1;
  }

  const auto blink_key = std::make_pair(stone, remaining_blinks);
  const auto blink_entry = blink_map.find(blink_key);
  if (blink_entry != blink_map.cend()) {
    return blink_entry->second;
  }

  ulong total_stones = 0;
  const Stones blinked_stones = blink_stone(stone);
  for (const ulong &blinked_stone : blinked_stones) {
    total_stones +=
        get_num_stones(blinked_stone, remaining_blinks - 1, blink_map);
  }

  blink_map.emplace(blink_key, total_stones);
  return total_stones;
}

[[nodiscard]] auto blink_stone(const ulong &stone) -> Stones {
  if (stone == 0) {
    return {1};
  }

  if (const auto digits = get_num_digits(stone); (digits & 1) == 0) {
    const auto [first_half, second_half] = split_stone(stone, digits);
    return {first_half, second_half};
  }

  return {stone * 2024};
}

[[nodiscard]] auto get_num_digits(ulong stone) noexcept -> uint {
  uint digits = 0;
  while (stone != 0) {
    ++digits;
    stone /= 10;
  }
  return digits;
}

[[nodiscard]] auto split_stone(const ulong &stone, const uint len)
    -> std::pair<ulong, ulong> {
  ulong first_half = 0;
  const auto exponent = len / 2;
  const auto divisor = static_cast<ulong>(std::powl(10, exponent));

  // Split the number into two parts.
  first_half = stone / divisor;
  ulong second_half = stone % divisor;

  return {first_half, second_half};
}
} // namespace
