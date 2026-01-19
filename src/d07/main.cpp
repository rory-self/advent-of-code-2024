#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>

namespace {
    struct Equation {
        const unsigned long result;
        const std::vector<unsigned long> terms;
    };
    using Operation = std::function<unsigned long(const unsigned long&, const unsigned long&)>;
    constexpr auto num_operations = 3;

    [[nodiscard]] auto read_equations_from_file(const std::string& file_path) -> std::vector<Equation> {
        std::vector<Equation> equations;

        auto file = std::ifstream(file_path);
        for (std::string line; std::getline(file, line);) {
            constexpr auto result_delimiter = ':';
            auto line_stream = std::stringstream(line);
            std::string result;
            std::getline(line_stream, result, result_delimiter);

            std::vector<unsigned long> terms;
            constexpr auto term_delimiter = ' ';
            for (std::string term; std::getline(line_stream, term, term_delimiter);) {
                if (term.empty()) {
                    continue;
                }

                const auto numeric_term = std::stoul(term);
                terms.push_back(numeric_term);
            }

            const auto numeric_result = std::stoul(result);
            equations.emplace_back(numeric_result, terms);
        }

        return equations;
    }

    [[nodiscard]] auto is_valid_equation(
        const Equation& equation,
        const std::array<Operation, num_operations>& operations
    ) -> bool {
        const auto& terms = equation.terms;
        const auto result = equation.result;

        if (terms.empty()) {
            return false;
        }

        const std::function<bool(std::size_t, const unsigned long&)> evaluate = [&](const std::size_t i, const unsigned long& curr_result) {
            if (i == terms.size() - 1) {
                return curr_result == result;
            }

            return std::ranges::any_of(operations, [&](const Operation& op) {
                return evaluate(i + 1, op(curr_result, terms[i + 1]));
            });
        };

        return evaluate(0, terms[0]);
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto equations = read_equations_from_file(file_path);

    const std::array<Operation, num_operations> operations = {
        [](const unsigned long& t1, const unsigned long& t2) { return t1 + t2; },
        [](const unsigned long& t1, const unsigned long& t2) { return t1 * t2; },
        [](const unsigned long& t1, const unsigned long& t2) {
            std::stringstream new_num;
            new_num << t1 << t2;

            return std::stoul(new_num.str());
        },
    };

    unsigned long test_value_sum = 0;
    for (const auto& equation : equations) {
        if (is_valid_equation(equation, operations)) {
            test_value_sum += equation.result;
        }
    }
    std::cout << test_value_sum << std::endl;
    return 0;
}
