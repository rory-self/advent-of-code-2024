#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>

namespace {
    struct Block {
        bool is_empty;
        uint id;
        std::size_t length;
    };

    [[nodiscard]] auto read_blocks_from_file(const std::string& file_path) -> std::vector<Block> {
        auto file = std::ifstream(file_path);
        std::string disk;
        std::getline(file, disk);

        std::vector<Block> blocks;
        auto is_free_space = false;
        uint id = 0;
        for (const auto& c : disk) {
            const std::size_t block_length = c - '0';
            for (std::size_t i = 0; i < block_length; ++i) {
                const auto new_block = Block{is_free_space, id, block_length};
                blocks.push_back(new_block);
            }

            if (not is_free_space) {
                ++id;
            }
            is_free_space = not is_free_space;
        }

        return blocks;
    }

    [[nodiscard]] auto block_compact_disk(std::vector<Block> blocks) -> std::vector<Block> {
        const auto is_free_space = [](const Block& block){ return block.is_empty; };
        for (auto it = blocks.rbegin(); it != blocks.rend(); ++it) {
            if (it->is_empty) {
                continue;
            }
            const auto forward_it = std::next(it).base();
            const auto first_empty_it = std::find_if(blocks.begin(), forward_it, is_free_space);

            std::swap(*it, *first_empty_it);
        }

        return blocks;
    }

    [[nodiscard]] auto file_compact_disk(std::vector<Block> blocks) -> std::vector<Block> {
        for (auto it = blocks.rbegin(); it != blocks.rend(); std::advance(it, it->length)) {
            if (it->is_empty) {
                continue;
            }

            const auto required_length = it->length;
            if (required_length == 0) {
                continue;
            }

            const auto forward_it = std::next(it).base();

            const auto is_valid_space = [required_length](const Block& block) {
                return block.is_empty and block.length >= required_length;
            };
            auto empty_it = std::find_if(blocks.begin(), forward_it, is_valid_space);
            if (empty_it == forward_it) {
                continue;
            }

            const auto free_space = empty_it->length;
            auto block_to_move_it = it;
            for (auto i = 0; i < required_length; ++i) {
                std::swap(*block_to_move_it, *empty_it);

                ++block_to_move_it;
                ++empty_it;
            }

            const std::size_t new_free_space = free_space - required_length;
            for (auto i = 0; i < new_free_space; ++i) {
                empty_it->length = new_free_space;
                ++empty_it;
            }
        }

        return blocks;
    }

    [[nodiscard]] auto calculate_checksum(const std::vector<Block>& blocks) -> unsigned long {
        unsigned long checksum = 0;
        for (std::size_t i = 0; i < blocks.size(); ++i) {
            const auto [is_empty, id, _] = blocks[i];
            if (is_empty) {
                continue;
            }

            checksum += id * i;
        }

        return checksum;
    }
}

auto main() -> int {
    const auto file_path = std::string{"input.txt"};
    const auto blocks = read_blocks_from_file(file_path);

    const auto block_compacted_disk = block_compact_disk(blocks);
    const auto block_move_checksum = calculate_checksum(block_compacted_disk);
    std::cout << "Checksum by moving blocks: " << block_move_checksum << '\n';

    const auto file_compacted_disk = file_compact_disk(blocks);
    const auto file_move_checksum = calculate_checksum(file_compacted_disk);
    std::cout << "Checksum by moving files:  " << file_move_checksum << '\n';

    return 0;
}