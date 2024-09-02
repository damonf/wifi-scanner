#include "byte_to_str.hpp"

#include <charconv>
#include <cstddef>
#include <expected>
#include <format>
#include <string>
#include <system_error>

namespace wifi_scanner::utils {

std::expected<size_t, std::string> byte_to_str(char *output, const unsigned char &byte) {

    auto [ptr, ec] = std::to_chars(output, output + 2, byte, 16);
    if (ec != std::errc()) {
        return std::unexpected{std::format("Error converting byte to hex: {}", byte)};
    }

    // If the result is only one digit add a leading zero
    if (ptr == output + 1) {
        *(output + 1)  = *(output);
        *(output) = '0';
    }

    return 2;
}

}
