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
        const std::vector<std::string>& instruction,
        const PrintingRules& rules
    ) -> int {
        std::vector<std::string> read_pages;

        for (const auto& page : instruction) {
            if (rules.contains(page)) {
                const auto& page_rules = rules.at(page);

                for (const auto& read_page : read_pages) {
                    if (page_rules.contains(read_page)) {
                        return 0;
                    }
                }
            }

            read_pages.push_back(page);
        }

        const auto& middle_page = instruction[instruction.size() / 2];
        return std::stoi(middle_page);
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    auto [rules, instructions] = process_printing_file(file_path);

    auto middle_page_sum = 0;
    for (const auto& instruction : instructions) {
        middle_page_sum += process_instruction(instruction, rules);
    }

    std::cout << middle_page_sum << '\n';
    return 0;
}