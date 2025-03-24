#include <fstream>
#include <vector>
#include <algorithm>
#include <climits>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

namespace {
    using Patterns = std::vector<std::string>;

    [[nodiscard]] auto towel_details_from_file(
        const std::string& file_path
    ) -> std::pair<Patterns, Patterns> {
        auto file = std::ifstream(file_path);

        // Collect available designs
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
        std::ranges::sort(available_patterns, [](const auto& p1, const auto& p2) { return p1.size() > p2.size(); });

        // Collect wanted designs
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
        const std::string& target_design,
        const std::vector<std::string>& available_patterns,
        std::unordered_set<std::string>& possible_designs,
        std::unordered_set<std::string>& impossible_designs
    ) -> bool {
        const auto is_best_pattern = [&](const auto& pattern) {
            if (pattern.size() > target_design.size()) {
                return false;
            }

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
                return is_possible_design(right_substr, available_patterns, possible_designs, impossible_designs);
            }
            if (is_end) {
                const auto left_substr = target_design.substr(0, substr_start);
                return is_possible_design(left_substr, available_patterns, possible_designs, impossible_designs);
            }

            const auto left_substr = target_design.substr(0, substr_start);
            const auto right_substr = target_design.substr(substr_end);
            return is_possible_design(left_substr, available_patterns, possible_designs, impossible_designs)
                and is_possible_design(right_substr, available_patterns, possible_designs, impossible_designs);
        };

        // Cache explored design possibilities for speedup
        if (possible_designs.contains(target_design)) {
            return true;
        }
        if (impossible_designs.contains(target_design)) {
            return false;
        }

        if (std::ranges::any_of(available_patterns, is_best_pattern)) {
            possible_designs.emplace(target_design);
            return true;
        }
        impossible_designs.emplace(target_design);
        return false;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto [available_designs, wanted_designs] = towel_details_from_file(file_path);

    uint num_possible_designs = 0;
    std::unordered_set<std::string> possible_designs;
    std::unordered_set<std::string> impossible_designs;
    for (const auto& target_design : wanted_designs) {
        if (is_possible_design(target_design, available_designs, possible_designs, impossible_designs)) {
            ++num_possible_designs;
        }
    }
    std::cout << num_possible_designs << " designs are possible\n";

    return 0;
}