#include "mac_to_str.hpp"
#include "byte_to_str.hpp"

#include <expected>
#include <span>
#include <string>
#include <cstddef>
#include <string_view>

namespace wifi_scanner::utils {

namespace {

    const constinit size_t MAC_BUF_MIN = 17;
    const constinit int MAC_ADDR_SIZE = 6;

}

std::expected<std::string_view, std::string> mac_to_str(
    const unsigned char *mac_addr
    , int mac_addr_len
    , std::span<char> buffer
) {

    if (mac_addr_len < MAC_ADDR_SIZE) {
        return std::unexpected("mac address buffer too small");
    }

    if (buffer.size() < MAC_BUF_MIN) {
        return std::unexpected("buffer too small");
    }

    auto bytes = std::span<const unsigned char>(
        mac_addr
        , MAC_ADDR_SIZE 
    );

    auto *output = buffer.data();
    size_t i = 0;
    for (const auto& byte : bytes) {

        auto res = byte_to_str(output + i, byte);
        if (!res) {
            return std::unexpected{res.error()};
        }

        i += res.value();

        if (i < MAC_BUF_MIN) {
            *(output + i) = ':';
            i++;
        }
    }

    return std::string_view{buffer.data(), i};
}

}
