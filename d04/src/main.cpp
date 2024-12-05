#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

namespace {
    using WordPuzzle = std::vector<std::vector<char>>;

    [[nodiscard]] auto read_word_puzzle_from_file(const std::string& file_path) -> WordPuzzle {
        auto word_puzzle = WordPuzzle{};
        constexpr auto height = std::size_t{140};
        word_puzzle.reserve(height);

        auto file = std::fstream {file_path};
        for (std::string line; std::getline(file, line);) {
            auto char_vec = std::vector(line.cbegin(), line.cend());
            word_puzzle.push_back(char_vec);
        }
        return word_puzzle;
    }

    [[nodiscard]] auto is_valid_coordinate(
        const int y,
        const int x
    ) -> bool {
        constexpr auto height = std::size_t{140};
        constexpr auto width = std::size_t{140};

        return y < height and x < width and y >= 0 and x >= 0;
    }

    [[nodiscard]] auto num_adjacent_words(
        const WordPuzzle& puzzle,
        const int y_pos,
        const int x_pos
    ) -> unsigned int {
        const auto word_to_find = std::string{"XMAS"};
        if (puzzle[y_pos][x_pos] != word_to_find[0]) {
            return 0;
        }

        constexpr std::pair<int, int> directions[8] = {
            {-1, 0},
            {0, 1},
            {0, -1},
            {1, 0},
            {1, 1},
            {-1, -1},
            {-1, 1},
            {1, -1}
        };

        const auto word_length = word_to_find.size();
        unsigned int num_adjacent = 0;

        for (const auto& direction : directions) {
            auto num_letters = 1;
            for (auto i = 1; i < word_length; ++i) {
                const auto [y_diff, x_diff] = direction;
                const auto new_y  = y_pos + y_diff * i;
                const auto new_x = x_pos + x_diff * i;

                if (!is_valid_coordinate(new_y, new_x) or word_to_find[i] != puzzle[new_y][new_x]) {
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

    [[nodiscard]] auto is_cross(
        const WordPuzzle& puzzle,
        const int y_pos,
        const int x_pos
    ) -> bool {
        const auto word_to_find = std::string{"MAS"};
        if (puzzle[y_pos][x_pos] != 'A') {
            return false;
        }

        using CoordsPair = std::pair<std::pair<int, int>, std::pair<int, int>>;
        constexpr CoordsPair corner_pairs[2] = {
            {
		{1, 1},
		{-1, -1},
            },
            {
                {-1, 1},
		{1, -1}
            }
        };

        const auto is_valid_pair = [puzzle, x_pos, y_pos](const CoordsPair& coords_pair) {
            const auto [y_diff1, x_diff1] = coords_pair.first;
            const auto [y_diff2, x_diff2] = coords_pair.second;

            const auto new_x1 = x_pos + x_diff1;
            const auto new_y1 = y_pos + y_diff1;
            const auto new_x2 = x_pos + x_diff2;
            const auto new_y2 = y_pos + y_diff2;

            if (!is_valid_coordinate(new_y1, new_x1) or !is_valid_coordinate(new_y2, new_x2)) {
                return false;
            }

            const auto is_valid_diagonal = [puzzle](const int y1, const int x1, const int y2, const int x2) {
                return puzzle[y1][x1] == 'M' and puzzle[y2][x2] == 'S';
            };

            return is_valid_diagonal(new_y1, new_x1, new_y2, new_x2) or is_valid_diagonal(new_y2, new_x2, new_y1, new_x1);
        };

        return std::ranges::all_of(corner_pairs, is_valid_pair);
    }
}

int main() {
    const auto file_path = std::string{"input.txt"};
    const auto word_puzzle = read_word_puzzle_from_file(file_path);

    unsigned int words_found = 0;
    unsigned int crosses_found = 0;
    for (auto y = 0; y < word_puzzle.size(); ++y) {
        for (auto x = 0; x < word_puzzle[y].size(); ++x) {
            words_found += num_adjacent_words(word_puzzle, y, x);

            if (is_cross(word_puzzle, y, x)) {
                ++crosses_found;
            }
        }
    }

    std::cout << "Times 'XMAS' found: " << words_found << '\n';
    std::cout << "Times 'X-MAS' found: " << crosses_found << '\n';
    return 0;
}
