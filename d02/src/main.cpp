#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>

namespace {
    auto read_reports_from_file(const std::string& file_path) -> std::vector<std::vector<int>> {
        auto file = std::ifstream(file_path);
        std::vector<std::vector<int>> reports;

        for (std::string line; std::getline(file, line);) {
            constexpr auto delimiter = ' ';
            auto line_stream = std::stringstream(line);

            std::vector<int> levels;
            for (std::string token; std::getline(line_stream, token, delimiter);) {
                const auto level = std::stoi(token);
                levels.push_back(level);
            }

            reports.push_back(levels);
        }

        return reports;
    }

    auto is_report_increasing(const std::vector<int>& report) -> bool {
        const auto increasing_pair = [](const auto e1, const auto e2) { return e1 < e2; };
        const auto first_increasing_pair = std::ranges::adjacent_find(report, increasing_pair);
        if (first_increasing_pair == report.cend()) {
            return false;
        }

        return std::adjacent_find(first_increasing_pair + 1, report.cend(), increasing_pair) != report.cend();
    }

    auto find_unsafe_level(const std::vector<int>& report) -> std::vector<int>::const_iterator {
        if (report.size() < 2) {
            return report.cend();
        }

        constexpr auto min_difference = 1;
        constexpr auto max_difference = 3;
        const auto is_increasing = is_report_increasing(report);
        const auto are_unsafe_levels = [is_increasing, min_difference, max_difference](const auto& e1, const auto& e2) {
            const auto difference = std::abs(e2 - e1);
            return e1 < e2 != is_increasing or difference > max_difference or difference < min_difference;
        };

        return std::ranges::adjacent_find(report, are_unsafe_levels);
    }

    auto damp_report(
        const std::vector<int>& report,
        const std::vector<int>::const_iterator& level_to_remove
    ) -> std::vector<int> {
        std::vector<int> damped_report;
        damped_report.reserve(report.size() - 1);
        damped_report.insert(damped_report.end(), report.cbegin(), level_to_remove);
        damped_report.insert(damped_report.end(), level_to_remove + 1, report.cend());

        return damped_report;
    }

    auto check_damped_report_safety(const std::vector<int>& report) -> bool {
        const auto first_unsafe_level = find_unsafe_level(report);
        if (first_unsafe_level == report.cend()) {
            return true;
        }

        auto damped_report = damp_report(report, first_unsafe_level);
        if (find_unsafe_level(damped_report) == damped_report.cend()) {
            return true;
        }

        damped_report = damp_report(report, first_unsafe_level + 1);
        return find_unsafe_level(damped_report) == damped_report.cend();
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto reports = read_reports_from_file(file_path);

    const auto num_safe_reports = std::ranges::count_if(reports, [](const auto& report) {
        return find_unsafe_level(report) == report.cend();
    });

    const auto num_safe_damped_reports = std::ranges::count_if(reports, check_damped_report_safety);

    std::cout << "Number of safe reports: " << num_safe_reports << '\n';
    std::cout << "Number of safe reports using the problem dampener: " << num_safe_damped_reports << '\n';
    return 0;
}
