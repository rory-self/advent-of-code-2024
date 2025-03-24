#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace {
    using Patterns = std::vector<std::string>;

    [[nodiscard]] auto towel_details_from_file(
        const std::string &file_path
    ) -> std::pair<Patterns, Patterns> {
        auto file = std::ifstream(file_path);

        // Collect available patterns
        Patterns available_patterns;
        std::string pattern_line;
        std::getline(file, pattern_line);
        auto pattern_stream = std::istringstream(pattern_line);

        constexpr auto pattern_delimiter = ',';
        for (std::string pattern; std::getline(pattern_stream, pattern, pattern_delimiter);) {
            if (pattern.empty()) {
                break;
            }
            if (pattern.front() == ' ') {
                pattern.erase(pattern.begin());
            }

            available_patterns.push_back(pattern);
        }
        std::ranges::sort(available_patterns, [](const auto &p1, const auto &p2) { return p1.size() > p2.size(); });

        // Collect target designs
        std::vector<std::string> wanted_designs;
        for (std::string design; std::getline(file, design);) {
            if (design.empty()) {
                continue;
            }
            wanted_designs.push_back(design);
        }

        return {available_patterns, wanted_designs};
    }

    [[nodiscard]] auto is_possible_design(
        const std::string &target_design,
        const std::vector<std::string>::const_iterator &patterns_start,
        const std::vector<std::string>::const_iterator &patterns_end,
        std::unordered_map<std::string, bool> &design_cache
    ) -> bool {
        const auto first_eligible_pattern = std::find_if(patterns_start, patterns_end, [target_design](const auto &p) {
            return p.size() <= target_design.size();
        });

        const auto is_best_pattern = [&target_design, first_eligible_pattern, patterns_end, &design_cache
                ](const auto &pattern) {
            const auto substr_start = target_design.find(pattern);
            if (substr_start == std::string::npos) {
                return false;
            }

            const auto is_start = substr_start == 0;
            const auto substr_end = substr_start + pattern.size();
            const auto is_end = substr_end == target_design.size();
            if (is_start and is_end) {
                return true;
            }

            if (is_start) {
                const auto right_substr = target_design.substr(substr_end);
                return is_possible_design(right_substr, first_eligible_pattern, patterns_end, design_cache);
            }
            if (is_end) {
                const auto left_substr = target_design.substr(0, substr_start);
                return is_possible_design(left_substr, first_eligible_pattern, patterns_end, design_cache);
            }

            const auto left_substr = target_design.substr(0, substr_start);
            const auto right_substr = target_design.substr(substr_end);
            return is_possible_design(left_substr, first_eligible_pattern, patterns_end, design_cache)
                   and is_possible_design(right_substr, first_eligible_pattern, patterns_end, design_cache);
        };

        // Cache explored design possibilities for speedup
        if (design_cache.contains(target_design)) {
            return design_cache[target_design];
        }

        const auto is_possible = std::any_of(first_eligible_pattern, patterns_end, is_best_pattern);
        design_cache[target_design] = is_possible;
        return is_possible;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto [patterns, designs] = towel_details_from_file(file_path);

    std::unordered_map<std::string, bool> design_cache;
    const auto num_possible_designs = std::ranges::count_if(designs, [&patterns, &design_cache](const auto &d) {
        return is_possible_design(d, patterns.cbegin(), patterns.cend(), design_cache);
    });
    std::cout << num_possible_designs << " designs are possible\n";

    return 0;
}
