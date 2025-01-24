#include "uint3_t.h"

[[nodiscard]] auto uint3_t::within_bounds() const noexcept -> bool {
    return data <= 7;
}

uint3_t::uint3_t(const unsigned long num) : data(num) {
    if (not within_bounds()) {
        throw std::invalid_argument("uint3_t must be between 0 and 7");
    }
}

[[nodiscard]] auto uint3_t::get() const noexcept -> uint8_t {
    return data;
}

[[nodiscard]] uint3_t::operator int() const {
    return data;
}

[[nodiscard]] auto uint3_t::operator==(const unsigned long long& other) const noexcept -> bool {
    return data == other;
}
