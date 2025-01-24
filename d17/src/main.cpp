#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

#include "uint3_t.h"

namespace {
    enum Instruction {
        Adv,
        Bxl,
        Bst,
        Jnz,
        Bxc,
        Out,
        Bdv,
        Cdv
    };

    struct DeviceData {
        const std::vector<uint3_t> program;
        std::size_t instruction_pointer;
        unsigned long reg_a;
        unsigned long reg_b;
        unsigned long reg_c;
    };

    [[nodiscard]] auto read_device_data_from_file(const std::string &file_path) -> DeviceData {
        std::ifstream file(file_path);
        std::string line;

        constexpr std::size_t value_index = 12;
        std::getline(file, line);
        const auto reg_a_str = line.substr(value_index);

        std::getline(file, line);
        const auto reg_b_str = line.substr(value_index);

        std::getline(file, line);
        const auto reg_c_str = line.substr(value_index);

        std::getline(file, line);
        std::getline(file, line);

        constexpr std::size_t first_instruction_index = 9;
        std::vector<uint3_t> program;
        const auto program_str = line.substr(first_instruction_index);
        auto instruction_stream = std::stringstream(program_str);
        while (not instruction_stream.eof()) {
            std::string instruction;
            std::getline(instruction_stream, instruction, ',');

            const auto instruction_num = std::stoul(instruction);
            const auto instruction_bits = uint3_t{instruction_num};
            program.push_back(instruction_bits);
        }

        return {
            program,
            0,
            std::stoul(reg_a_str),
            std::stoul(reg_b_str),
            std::stoul(reg_c_str)
        };
    }

    [[nodiscard]] auto evaluate_combo_operand(const uint3_t operand, const DeviceData &device_data) -> unsigned long {
        switch (operand.get()) {
            case 0:
            case 1:
            case 2:
            case 3:
                return static_cast<int>(operand);
            case 4:
                return device_data.reg_a;
            case 5:
                return device_data.reg_b;
            case 6:
                return device_data.reg_c;
            case 7:
                throw std::invalid_argument("Operand cannot be '7', value is reserved");
            default:
                throw std::invalid_argument("Invalid operand, must fit an unsigned 3 bit integer");
        }
    }

    auto execute_division(const uint3_t operand, const DeviceData &device_data, unsigned long &dest_reg) {
        const auto numerator = device_data.reg_a;
        const auto combo_operand = evaluate_combo_operand(operand, device_data);
        const auto denominator = std::pow(2, combo_operand);

        dest_reg = numerator / static_cast<unsigned long>(denominator);
    }

    [[nodiscard]] auto
    execute_instructions(DeviceData device_data) -> std::pair<std::vector<unsigned long>, DeviceData> {
        auto &program = device_data.program;
        auto &instruction_pointer = device_data.instruction_pointer;

        std::vector<unsigned long> outputs;
        while (instruction_pointer < program.size()) {
            const auto raw_instruction = program.at(instruction_pointer);
            const auto instruction = static_cast<Instruction>(raw_instruction.get());

            const auto operand = program.at(instruction_pointer + 1);

            switch (instruction) {
                case Bxl: {
                    const auto literal = operand.get();
                    device_data.reg_b = literal ^ device_data.reg_b;
                    break;
                }
                case Bst: {
                    const auto combo_operand = evaluate_combo_operand(operand, device_data);
                    device_data.reg_b = combo_operand % 8;
                    break;
                }
                case Jnz: {
                    if (device_data.reg_a == 0) {
                        break;
                    }
                    instruction_pointer = operand.get();
                    continue;
                }
                case Bxc: {
                    device_data.reg_b = device_data.reg_b ^ device_data.reg_c;
                    break;
                }
                case Out: {
                    const auto combo_operand = evaluate_combo_operand(operand, device_data) % 8;
                    outputs.push_back(combo_operand);
                    break;
                }
                case Adv: {
                    execute_division(operand, device_data, device_data.reg_a);
                    break;
                }
                case Bdv: {
                    execute_division(operand, device_data, device_data.reg_b);
                    break;
                }
                case Cdv: {
                    execute_division(operand, device_data, device_data.reg_c);
                    break;
                }
                default:
                    assert("Invalid instruction");
            }

            instruction_pointer += 2;
        }

        return {outputs, device_data};
    }

    [[nodiscard]] auto find_min_copying_condition(DeviceData device_data) -> unsigned long {
        const auto &program = device_data.program;
        auto &reg_a = device_data.reg_a;

        const auto is_program_copy = [program](const auto &outputs) -> bool {
            if (outputs.size() != program.size()) {
                return false;
            }

            for (std::size_t i = 0; i < outputs.size(); ++i) {
                if (program[i] != outputs[i]) {
                    return false;
                }
            }
            return true;
        };

        reg_a = 0;
        auto outputs = execute_instructions(device_data).first;
        while (not is_program_copy(outputs)) {
            reg_a += 1;
            outputs = execute_instructions(device_data).first;
        }

        return reg_a;
    }
}

auto main() -> int {
    const auto input_file_path = std::string{"./test.txt"};
    const auto initial_device_data = read_device_data_from_file(input_file_path);

    constexpr auto separator = "----------------------\n";
    std::cout << separator << "Program Results:\n";

    const auto [outputs, device_data] = execute_instructions(initial_device_data);
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        const auto delimiter = i == outputs.size() - 1 ? '\n' : ',';
        std::cout << outputs.at(i) << delimiter;
    }

    std::cout << "Register A: " << device_data.reg_a << '\n';
    std::cout << "Register B: " << device_data.reg_b << '\n';
    std::cout << "Register C: " << device_data.reg_c << '\n';
    std::cout << separator;

    const auto min_copying_value = find_min_copying_condition(initial_device_data);
    std::cout << "Minimum copying value: " << min_copying_value << '\n';
    return 0;
}
