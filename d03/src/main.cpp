#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

namespace {
    [[nodiscard]] auto read_corrupted_memory_from_file(const std::string& file_path) -> std::string {
        auto file = std::ifstream(file_path);

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    [[nodiscard]] auto extract_mul_parameters(const std::string& mul_func) -> std::pair<int, int> {
        constexpr auto parameters_index = 4;
        constexpr auto delimiter = ',';
        const auto delimiter_index = mul_func.find_first_of(delimiter);
        const auto arg1_string = mul_func.substr(parameters_index, delimiter_index);
        const auto arg2_string = mul_func.substr(delimiter_index + 1);

        const auto arg1 = std::stoi(arg1_string);
        const auto arg2 = std::stoi(arg2_string);
        return {arg1, arg2};
    }

    [[nodiscard]] auto process_corrupted_mul(const std::string& memory) -> int {
        const auto multiply_pattern = std::regex{R"((do\(\)|don't\(\)|mul\([0-9]{1,3},[0-9]{1,3}\)))"};
        const auto do_func = std::string{"do()"};
        const auto dont_func = std::string{"don't()"};

        auto enabled = true;
        auto product_sum = 0;
        const auto first_match_it = std::sregex_iterator(memory.begin(), memory.end(), multiply_pattern);
        for (auto it = first_match_it; it != std::sregex_iterator(); ++it) {
            const auto func_as_str = it->str();

            if (func_as_str == do_func) {
                enabled = true;
            } else if (func_as_str == dont_func) {
                enabled = false;
            } else if (enabled) {
                const auto [arg1, arg2] = extract_mul_parameters(func_as_str);
                product_sum += arg1 * arg2;
            }
        }

        return product_sum;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto corrupted_memory = read_corrupted_memory_from_file(file_path);

    const auto result = process_corrupted_mul(corrupted_memory);

    std::cout << result << '\n';
    return 0;
}