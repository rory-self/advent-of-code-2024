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

    [[nodiscard]] auto read_disk_from_file(const std::string& file_path) -> std::vector<Block> {
        auto file = std::ifstream(file_path);
        std::string disk;
        std::getline(file, disk);

        std::vector<Block> blocks;
        auto is_free_space = false;
        uint curr_id = 0;
        for (const auto& c : disk) {
            const std::size_t block_length = c - '0';
            for (std::size_t i = 0; i < block_length; ++i) {
                const auto id = is_free_space ? 0 : curr_id;
                const auto new_block = Block{is_free_space, id, block_length};
                blocks.push_back(new_block);
            }

            if (not is_free_space) {
                ++curr_id;
            }
            is_free_space = not is_free_space;
        }

        return blocks;
    }

    [[nodiscard]] auto block_compact_disk(std::vector<Block> disk) -> std::vector<Block> {
        const auto is_free_space = [](const Block& block){ return block.is_empty; };
        for (auto block_it = disk.rbegin(); block_it != disk.rend(); ++block_it) {
            if (block_it->is_empty) {
                continue;
            }
            const auto forward_block_it = std::next(block_it).base();
            const auto empty_it = std::find_if(disk.begin(), forward_block_it, is_free_space);

            std::swap(*block_it, *empty_it);
        }

        return disk;
    }

    [[nodiscard]] auto file_compact_disk(std::vector<Block> disk) -> std::vector<Block> {
        for (auto file_it = disk.rbegin(); file_it != disk.rend();) {
            const auto file_length = file_it->length;
            if (file_length == 0) {
                ++file_it;
                continue;
            }
            if (file_it->is_empty) {
                std::advance(file_it, file_length);
                continue;
            }

            // Check for empty space of required length
            const auto forward_file_it = std::next(file_it).base();
            const auto is_valid_space = [file_length](const Block& block) {
                return block.is_empty and block.length >= file_length;
            };
            auto empty_it = std::find_if(disk.begin(), forward_file_it, is_valid_space);
            if (empty_it == forward_file_it) {
                std::advance(file_it, file_length);
                continue;
            }

            // Switch blocks in file
            const auto free_space = empty_it->length - file_length;
            auto block_to_move_it = file_it;
            for (auto i = 0; i < file_length; ++i, ++block_to_move_it, ++empty_it) {
                std::swap(*block_to_move_it, *empty_it);
            }

            for (auto i = 0; i < free_space; ++i, ++empty_it) {
                empty_it->length = free_space;
            }

            std::advance(file_it, file_length);
        }

        return disk;
    }

    [[nodiscard]] auto calculate_checksum(const std::vector<Block>& disk) -> unsigned long {
        unsigned long checksum = 0;
        for (std::size_t i = 0; i < disk.size(); ++i) {
            const auto [is_empty, id, _] = disk[i];
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
    const auto disk = read_disk_from_file(file_path);

    const auto block_compacted_disk = block_compact_disk(disk);
    const auto block_move_checksum = calculate_checksum(block_compacted_disk);
    std::cout << "Checksum by moving blocks: " << block_move_checksum << '\n';

    const auto file_compacted_disk = file_compact_disk(disk);
    const auto file_move_checksum = calculate_checksum(file_compacted_disk);
    std::cout << "Checksum by moving files:  " << file_move_checksum << '\n';

    return 0;
}