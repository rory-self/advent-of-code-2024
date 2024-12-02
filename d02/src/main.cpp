#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

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

    auto check_report_safety(const std::vector<int>& report) -> bool {
        if (report.size() <= 2) {
            return true;
        }

        constexpr auto min_difference = 1;
        constexpr auto max_difference = 3;
        const auto is_increasing = *report.cbegin() < *(report.cbegin() + 1);
        const auto are_unsafe_levels = [is_increasing, max_difference](const auto e1, const auto e2) {
            const auto difference = std::abs(e2 - e1);
            return e1 < e2 != is_increasing or difference > max_difference or difference < min_difference;
        };

        return std::ranges::adjacent_find(report, are_unsafe_levels) == report.cend();
    }
}

int main() {
    const auto file_path = std::string{"input.txt"};
    const auto reports = read_reports_from_file(file_path);

    const auto num_safe_reports = std::ranges::count_if(reports, check_report_safety);

    std::cout << "Number of safe reports: " << num_safe_reports << '\n';
    return 0;
}