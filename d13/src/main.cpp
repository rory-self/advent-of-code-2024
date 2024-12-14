#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <fstream>
#include <regex>
#include <set>

namespace {
    using Matrix2x2 = std::array<std::array<long, 2>, 2>;
    struct GameSpecs {
        Matrix2x2 button_specs;
        std::array<long, 2> prize_pos;
    };

    [[nodiscard]] auto calculate_2x2_discriminant(const Matrix2x2& matrix) -> long {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    }

    /**
     * Solves two unknown variables using Cramer's method
     * @param matrix - 2x2 matrix holding variable coefficients
     * @param results - answers of each simultaneous equation
     * @return value of the two unknown variables in a std::pair
     */
    [[nodiscard]] auto solve_matrix_variables(const Matrix2x2& matrix, const std::array<long, 2>& results) -> std::pair<double, double> {
        const auto raw_discriminant = calculate_2x2_discriminant(matrix);
        const auto raw_discriminant_as_double = static_cast<double>(raw_discriminant);

        const auto mutated_matrix1 = Matrix2x2{{{results[0], matrix[0][1]}, {results[1], matrix[1][1]}}};
        const auto discriminant1 = calculate_2x2_discriminant(mutated_matrix1);
        const auto variable1 = static_cast<double>(discriminant1) / raw_discriminant_as_double;

        const auto mutated_matrix2 = Matrix2x2{{{matrix[0][0], results[0]}, {matrix[1][0], results[1]}}};
        const auto discriminant2 = calculate_2x2_discriminant(mutated_matrix2);
        const auto variable2 = static_cast<double>(discriminant2) / raw_discriminant_as_double;

        return {variable1, variable2};
    }

    [[nodiscard]] auto extract_positional_args_from_file_line(
        std::ifstream& file,
        const std::regex& pattern
    ) -> std::pair<long, long> {
        std::string haystack;
        std::getline(file, haystack);

        auto it = std::regex_iterator(haystack.cbegin(), haystack.cend(), pattern);
        const auto match1_str = it->str();

        ++it;
        const auto match2_str = it->str();

        const auto float1 = std::stof(match1_str);
        const auto float2 = std::stof(match2_str);
        return {float1, float2};
    }

    [[nodiscard]] auto read_crane_games_from_file(
        const std::string& file_path,
        const bool adjust_prize_pos
    ) -> std::vector<GameSpecs> {
        std::vector<GameSpecs> all_game_specs;

        const auto numeric_pattern = std::regex{"[0-9]+"};

        auto file = std::ifstream(file_path);
        std::string buffer;
        do {
            const auto [button_a_x, button_a_y] = extract_positional_args_from_file_line(file, numeric_pattern);
            const auto [button_b_x, button_b_y] = extract_positional_args_from_file_line(file, numeric_pattern);
            auto [prize_x, prize_y] = extract_positional_args_from_file_line(file, numeric_pattern);

            if (adjust_prize_pos) {
                constexpr auto adjustment_value = 10000000000000;
                prize_x += adjustment_value;
                prize_y += adjustment_value;
            }

            const auto button_specs = Matrix2x2{{{button_a_x, button_b_x}, {button_a_y, button_b_y}}};
            const auto prize_pos = std::array<long, 2>{{prize_x, prize_y}};
            all_game_specs.push_back({button_specs, prize_pos});

            std::getline(file, buffer);
        } while (not file.eof());

        return all_game_specs;
    }

    [[nodiscard]] auto tokens_required_for_max_prizes(const std::vector<GameSpecs>& game_specs) -> unsigned long {
        const auto is_impossible_to_press = [](const double& num_presses) {
            return num_presses != std::floor(num_presses) or num_presses < 0;
        };

        unsigned long tokens_required = 0;
        for (const auto&[button_specs, prize_pos] : game_specs) {
            const auto [a_presses, b_presses] = solve_matrix_variables(button_specs, prize_pos);
            if (is_impossible_to_press(a_presses) or is_impossible_to_press(b_presses)) {
                continue;
            }

            tokens_required += static_cast<unsigned long>(a_presses) * 3 + static_cast<unsigned long>(b_presses);
        }

        return tokens_required;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto normal_game_specs = read_crane_games_from_file(file_path, false);
    const auto difficult_game_specs = read_crane_games_from_file(file_path, true);

    const unsigned long normal_tokens_required = tokens_required_for_max_prizes(normal_game_specs);
    const unsigned long adjusted_tokens_required = tokens_required_for_max_prizes(difficult_game_specs);

    std::cout << "Tokens required for normal prize position: " << normal_tokens_required << '\n';
    std::cout << "Tokens required for adjusted prize position: " << adjusted_tokens_required << '\n';
    return 0;
}