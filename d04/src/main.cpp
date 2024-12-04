#include <cstdint>
#include <iostream>
#include <vector>
#include <fstream>

namespace {
    using WordPuzzle = std::vector<std::vector<char>>;

    [[nodiscard]] auto read_word_puzzle_from_file(const std::string& file_path) -> WordPuzzle {
        auto word_puzzle = WordPuzzle{};

        auto file = std::fstream {file_path};
        for (std::string line; std::getline(file, line);) {
            auto char_vec = std::vector(line.begin(), line.end());
            word_puzzle.push_back(char_vec);
        }
        return word_puzzle;
    }

    [[nodiscard]] auto num_adjacent_words(
        const WordPuzzle& puzzle,
        const int y_pos,
        const int x_pos
    ) -> unsigned int {
        const auto word_to_find = std::string{"XMAS"};
        const auto word_length = word_to_find.size();
        unsigned int num_adjacent = 0;

        constexpr std::pair<int, int> direction_vectors[8] = {
            {-1, 0},
            {0, 1},
            {0, -1},
            {1, 0},
            {1, 1},
            {-1, -1},
            {-1, 1},
            {1, -1}
        };
        const auto puzzle_height = puzzle.size();
        const auto puzzle_width = puzzle[0].size();
        const auto valid_coords = [puzzle_height, puzzle_width](const int y, const int x) {
            return y < puzzle_height and x < puzzle_width and y >= 0 and x >= 0;
        };

        for (const auto& direction : direction_vectors) {
            auto num_letters = 0;
            for (auto i = 0; i < word_length; ++i) {
                const auto [y_diff, x_diff] = direction;
                const auto new_y  = y_pos + y_diff * i;
                const auto new_x = x_pos + x_diff * i;

                if (!valid_coords(new_y, new_x) or word_to_find[i] != puzzle[new_y][new_x]) {
                    break;
                }
                ++num_letters;
            }
            if (num_letters == word_length) {
                ++num_adjacent;
            }
        }
        return num_adjacent;
    }
}

int main() {
    const auto file_path = std::string{"input.txt"};
    const auto word_puzzle = read_word_puzzle_from_file(file_path);

    auto words_found = 0;
    for (auto y = 0; y < word_puzzle.size(); ++y) {
        for (auto x = 0; x < word_puzzle[y].size(); ++x) {
            words_found += num_adjacent_words(word_puzzle, y, x);
        }
    }

    std::cout << words_found << '\n';
    return 0;
}