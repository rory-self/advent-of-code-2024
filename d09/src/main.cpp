#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <ranges>

namespace {
    struct Block {
        bool is_empty;
        uint id;
    };

    [[nodiscard]] auto read_blocks_from_file(const std::string& file_path) -> std::vector<Block> {
        auto file = std::ifstream(file_path);
        std::string disk;
        std::getline(file, disk);

        std::vector<Block> blocks;
        auto is_free_space = false;
        uint id = 0;
        for (const auto& c : disk) {
            const uint block_length = c - '0';
            for (auto i = 0; i < block_length; ++i) {
                const auto new_block = Block{is_free_space, id};
                blocks.push_back(new_block);
            }

            if (not is_free_space) {
                ++id;
            }
            is_free_space = not is_free_space;
        }

        return blocks;
    }

    auto compact_disk(std::vector<Block>& blocks) {
        const auto is_free_space = [](const Block& block){ return block.is_empty; };
        auto first_empty_it = std::ranges::find_if(blocks, is_free_space);
        for (auto it = blocks.rbegin(); it != blocks.rend(); ++it) {
            if (it->is_empty) {
                continue;
            }

            if (first_empty_it != blocks.end()) {
                std::swap(*it, *first_empty_it);
                first_empty_it = std::find_if(first_empty_it + 1, blocks.end(), is_free_space);
            }
        }
    }

    [[nodiscard]] auto calculate_checksum(const std::vector<Block>& blocks) -> unsigned long {
        unsigned long checksum = 0;
        for (std::size_t i = 0; i < blocks.size(); ++i) {
            const auto [is_empty, id] = blocks[i];
            if (is_empty) {
                break;
            }

            checksum += id * i;
        }

        return checksum;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    auto blocks = read_blocks_from_file(file_path);
    compact_disk(blocks);
    const auto checksum = calculate_checksum(blocks);

    std::cout << checksum << '\n';
    return 0;
}