#ifndef INT3_T_H
#define INT3_T_H

#include <cstdint>
#include <stdexcept>

class uint3_t {
    uint8_t data;

    [[nodiscard]] auto within_bounds() const noexcept -> bool;
public:
    explicit uint3_t(unsigned long num);

    [[nodiscard]] auto get() const noexcept -> uint8_t;

    explicit operator int() const;
    [[nodiscard]] auto operator==(const unsigned long long& other) const noexcept -> bool;
};


#endif //INT3_T_H
