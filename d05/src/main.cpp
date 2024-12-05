#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace {
    using PrintingRules = std::unordered_map<std::string, std::unordered_set<std::string>>;
    using Instructions = std::vector<std::vector<std::string>>;

    [[nodiscard]] auto read_printing_rules(std::fstream& file) -> PrintingRules {
        PrintingRules rules;

        for (std::string line; std::getline(file, line);) {
            if (line.empty()) {
                break;
            }

            constexpr auto delimiter = '|';
            auto line_stream = std::stringstream(line);
            std::string first_page, second_page;
            std::getline(line_stream, first_page, delimiter);
            std::getline(line_stream, second_page);

            if (rules.contains(first_page)) {
                rules.at(first_page).insert(second_page);
            } else {
                const auto rule_set = std::unordered_set{second_page};
                rules.emplace(first_page, rule_set);
            }
        }

        return rules;
    }

    [[nodiscard]] auto read_printing_instructions(std::fstream& file) -> Instructions {
        Instructions instructions;
        for (std::string line; std::getline(file, line);) {
            std::vector<std::string> instruction;

            constexpr auto delimiter = ',';
            auto line_stream = std::stringstream(line);
            for (std::string page; std::getline(line_stream, page, delimiter);) {
                instruction.push_back(page);
            }
            instructions.push_back(instruction);
        }

        return instructions;
    }

    [[nodiscard]] auto process_printing_file(
        const std::string& file_path
    ) -> std::pair<PrintingRules, Instructions> {
        auto file = std::fstream(file_path);
        const auto rules = read_printing_rules(file);
        const auto instructions = read_printing_instructions(file);

        return {rules, instructions};
    }

    [[nodiscard]] auto process_instruction(
        std::vector<std::string>& instruction,
        const PrintingRules& rules,
        const bool correction
    ) -> int;

    [[nodiscard]] auto process_corrected_instruction(
        const std::vector<std::string>::iterator& page1,
        const std::vector<std::string>::iterator& page2,
        std::vector<std::string>& instruction,
        const PrintingRules& rules,
        const bool correction
    ) -> int {
        std::iter_swap(page1, page2);
        return process_instruction(instruction, rules, correction);
    }

    [[nodiscard]] auto process_instruction(
        std::vector<std::string>& instruction,
        const PrintingRules& rules,
        const bool correction
    ) -> int {
        std::vector<std::vector<std::string>::iterator> read_pages;

        for (auto page_it = instruction.begin(); page_it != instruction.end(); ++page_it) {
            if (rules.contains(*page_it)) {
                const auto& page_rules = rules.at(*page_it);

                for (const auto& read_it : read_pages) {
                    if (page_rules.contains(*read_it) and correction) {
                        return process_corrected_instruction(read_it, page_it, instruction, rules, correction);
                    }
                    if (page_rules.contains(*read_it) and not correction) {
                        return 0;
                    }
                }
            }

            read_pages.push_back(page_it);
        }

        const auto& middle_page = instruction[instruction.size() / 2];
        return std::stoi(middle_page);
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    auto [rules, instructions] = process_printing_file(file_path);

    auto middle_page_sum = 0;
    auto corrected_sum = 0;
    for (auto& instruction : instructions) {
        middle_page_sum += process_instruction(instruction, rules, false);
        corrected_sum += process_instruction(instruction, rules, true);
    }

    const auto only_corrected_sum = corrected_sum - middle_page_sum;

    std::cout << "Non-corrections:" << middle_page_sum << '\n';
    std::cout << "Corrections:" << only_corrected_sum << '\n';
    return 0;
}