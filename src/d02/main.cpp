// Libraries //
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace {
// Types //
using Report = std::vector<int>;

// Prototypes //
  [[nodiscard]] auto read_reports_from_file(const std::string& filepath) -> std::vector<Report>;
  [[nodiscard]] auto is_report_increasing(const Report& report) -> bool;
  [[nodiscard]] auto find_unsafe_level(const Report& report) -> Report::const_iterator;
  [[nodiscard]] auto damp_report(const Report& report, const Report::const_iterator& level_to_remove) -> Report;
  [[nodiscard]] auto check_damped_report_safety(const Report& report) -> bool;
} // namespace

// Implementation //
auto main(const int argc, const char **argv) -> int {
  if (argc < 2) {
    throw "No input filepath provided.";
  }

  const std::vector<Report> reports = read_reports_from_file(argv[1]);

  const unsigned int num_safe_reports = std::ranges::count_if(reports, [](const auto& report) {
      return find_unsafe_level(report) == report.cend();
  });
  std::cout << "Number of safe reports: " << num_safe_reports << '\n';

  const unsigned int num_safe_damped_reports = std::ranges::count_if(reports, check_damped_report_safety);
  std::cout << "Number of safe reports using the problem dampener: " << num_safe_damped_reports << '\n';
  return 0;
}

namespace {
auto read_reports_from_file(const std::string& file_path) -> std::vector<Report> {
  auto file = std::ifstream(file_path);
  std::vector<Report> reports;

  for (std::string line; std::getline(file, line);) {
      constexpr auto delimiter = ' ';
      auto line_stream = std::stringstream(line);

      Report report;
      for (std::string token; std::getline(line_stream, token, delimiter);) {
          const auto level = std::stoi(token);
          report.push_back(level);
      }

      reports.push_back(report);
  }

  return reports;
}

auto is_report_increasing(const Report& report) -> bool {
  const auto is_increasing_pair = [](const auto e1, const auto e2) { return e1 < e2; };
  const auto first_increasing_pair_it = std::ranges::adjacent_find(report, is_increasing_pair);
  if (first_increasing_pair_it == report.cend()) {
      return false;
  }

  return std::adjacent_find(first_increasing_pair_it + 1, report.cend(), is_increasing_pair) != report.cend();
}

auto find_unsafe_level(const Report& report) -> Report::const_iterator {
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

auto check_damped_report_safety(const Report& report) -> bool {
  const auto first_unsafe_level = find_unsafe_level(report);
  if (first_unsafe_level == report.cend()) {
      return true;
  }

  Report damped_report = damp_report(report, first_unsafe_level);
  if (find_unsafe_level(damped_report) == damped_report.cend()) {
      return true;
  }

  damped_report = damp_report(report, first_unsafe_level + 1);
  return find_unsafe_level(damped_report) == damped_report.cend();
}

auto damp_report(const Report& report, const Report::const_iterator& level_to_remove) -> Report {
  Report damped_report;
  damped_report.reserve(report.size() - 1);
  damped_report.insert(damped_report.end(), report.cbegin(), level_to_remove);
  damped_report.insert(damped_report.end(), level_to_remove + 1, report.cend());

  return damped_report;
}
} // namespace
