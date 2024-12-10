#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <unordered_set>
#include <ranges>

namespace {
    struct Coordinates {
        int x;
        int y;

        [[nodiscard]] auto operator==(const Coordinates& other) const -> bool = default;
    };

    struct CoordinatesHash {
        [[nodiscard]] auto operator()(const Coordinates& coords) const -> std::size_t {
            const auto h1 = std::hash<int>()(coords.x);
            const auto h2 = std::hash<int>()(coords.y);

            // Combine field hash values
            return h1 ^ h2 << 1;
        }
    };

    using Frequencies = std::unordered_map<char, std::vector<Coordinates>>;
    using Antinodes = std::unordered_set<Coordinates, CoordinatesHash>;

    [[nodiscard]] auto read_frequencies_from_file(const std::string& file_path) -> Frequencies {
        Frequencies frequencies;
        auto file = std::fstream(file_path);
        auto coords = Coordinates{0, 0};

        for (std::string line; std::getline(file, line);) {
            for (const auto& c : line) {
                if (constexpr auto empty_pos = '.'; c == empty_pos) {
                    ++coords.x;
                    continue;
                }

                if (frequencies.contains(c)) {
                    frequencies[c].push_back(coords);
                } else {
                    frequencies.emplace(c, std::vector{coords});
                }
                ++coords.x;
            }
            coords.x = 0;
            ++coords.y;
        }

        return frequencies;
    }

    [[nodiscard]] auto count_antinodes(const Frequencies& frequencies) -> std::size_t {
        Antinodes antinodes;

        const auto try_insert_antinode = [&antinodes](const int x, const int y) {
            const auto within_bounds = [](const int x_pos, const int y_pos) {
                constexpr auto max_x = 50;
                constexpr auto max_y = 50;
                return x_pos >= 0 and y_pos >= 0 and x_pos < max_x and y_pos < max_y;
            };

            if (const auto coords = Coordinates{x, y}; within_bounds(x, y)) {
                antinodes.insert(coords);
                return true;
            }

            return false;
        };

        for (const auto& coordinates : frequencies | std::views::values) {
            for (auto it1 = coordinates.cbegin(); it1 != coordinates.cend(); ++it1) {
                for (auto it2 = std::next(it1); it2 != coordinates.cend(); ++it2) {
                    const auto coords1 = *it1;
                    const auto coords2 = *it2;
                    if (coords1 == coords2) {
                        continue;
                    }

                    const auto x_diff = coords2.x - coords1.x;
                    const auto y_diff = coords2.y - coords1.y;

                    auto num_wavelengths = 0;
                    bool valid_antinode;
                    do {
                        const auto antinode_x = coords1.x - x_diff * num_wavelengths;
                        const auto antinode_y = coords1.y - y_diff * num_wavelengths;

                        valid_antinode = try_insert_antinode(antinode_x, antinode_y);
                        ++num_wavelengths;
                    } while (valid_antinode);

                    num_wavelengths = 0;
                    do {
                        const auto antinode_x = coords2.x + x_diff * num_wavelengths;
                        const auto antinode_y = coords2.y + y_diff * num_wavelengths;

                        valid_antinode = try_insert_antinode(antinode_x, antinode_y);
                        ++num_wavelengths;
                    } while (valid_antinode);
                }
            }
        }

        return antinodes.size();
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto frequencies = read_frequencies_from_file(file_path);
    const auto num_antinodes = count_antinodes(frequencies);

    std::cout << num_antinodes << '\n';
    return 0;
}