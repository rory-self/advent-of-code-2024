#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>

namespace {
    constexpr auto height = std::size_t{140};
    constexpr auto width = std::size_t{140};
    using WordPuzzle = std::array<std::array<char, width>, height>;

    [[nodiscard]] auto read_word_puzzle_from_file(const std::string& file_path) -> WordPuzzle {
        auto word_puzzle = WordPuzzle{};

        auto file = std::fstream {file_path};
        auto i = 0;
        for (std::string line; std::getline(file, line);) {
            auto j = 0;
            for (const char c : line) {
                word_puzzle[i][j++] = c;
            }
            ++i;
        }
        return word_puzzle;
    }

    [[nodiscard]] auto is_invalid_coordinate(const int y, const int x) noexcept -> bool {
        return y >= height or x >= width or y < 0 or x < 0;
    }

    [[nodiscard]] auto num_adjacent_words(
        const WordPuzzle& puzzle,
        const std::size_t y_pos,
        const std::size_t x_pos
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

        const auto x = static_cast<int>(x_pos);
        const auto y = static_cast<int>(y_pos);
        const auto word_length = word_to_find.size();
        unsigned int num_adjacent = 0;

        for (const auto& direction : directions) {
            auto num_letters = 1;
            for (auto i = 1; i < word_length; ++i) {
                const auto [y_diff, x_diff] = direction;
                const auto new_y  = y + y_diff * i;
                const auto new_x = x + x_diff * i;

                if (is_invalid_coordinate(new_y, new_x) or word_to_find[i] != puzzle[new_y][new_x]) {
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

    [[nodiscard]] auto is_cross(const WordPuzzle& puzzle, const std::size_t y_pos, const std::size_t x_pos) -> bool {
        if (constexpr auto center_char = 'A'; puzzle[y_pos][x_pos] != center_char) {
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

        const auto y = static_cast<int>(y_pos);
        const auto x = static_cast<int>(x_pos);
        const auto is_valid_pair = [puzzle, x, y](const CoordsPair& coords_pair) {
            const auto [y_diff1, x_diff1] = coords_pair.first;
            const auto [y_diff2, x_diff2] = coords_pair.second;

            const auto new_x1 = x + x_diff1;
            const auto new_y1 = y + y_diff1;
            const auto new_x2 = x + x_diff2;
            const auto new_y2 = y + y_diff2;

            if (is_invalid_coordinate(new_y1, new_x1) or is_invalid_coordinate(new_y2, new_x2)) {
                return false;
            }

            const auto is_valid_diagonal = [puzzle](const int y1, const int x1, const int y2, const int x2) {
                constexpr auto start_char = 'M';
                constexpr auto end_char = 'S';
                return puzzle[y1][x1] == start_char and puzzle[y2][x2] == end_char;
            };

            return is_valid_diagonal(new_y1, new_x1, new_y2, new_x2)
                or is_valid_diagonal(new_y2, new_x2, new_y1, new_x1);
        };

        return std::ranges::all_of(corner_pairs, is_valid_pair);
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto word_puzzle = read_word_puzzle_from_file(file_path);

    unsigned int words_found = 0;
    unsigned int crosses_found = 0;
    for (std::size_t y = 0; y < word_puzzle.size(); ++y) {
        for (std::size_t x = 0; x < word_puzzle[y].size(); ++x) {
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
