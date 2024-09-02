#include "parse_ssid.hpp"
#include "byte_to_str.hpp"

#include <cstring>
#include <expected>
#include <iostream>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <cstddef>
#include <cctype>

namespace wifi_scanner::parsers {

namespace {

    const constinit int SSID_ID = 0;
    const constinit size_t IE_HEADER_LEN = 2;
    const constinit size_t SSID_MAX_LEN = 32;
    const constinit size_t SSID_BUF_MIN = SSID_MAX_LEN * 4L;

    using namespace wifi_scanner::utils;
    using byte_span = std::span<const unsigned char>;

    bool is_valid_ssid(byte_span field) {
    
        const auto len = field[1];

        if (field[0] != SSID_ID) {
            return false;
        }

        if (len + IE_HEADER_LEN > field.size()) {
            std::cout << "SSID length exceeds the field length: " << len << std::endl;
            return false;
        }

        if (len > SSID_MAX_LEN) {
            std::cout << "SSID too long: " << len << std::endl;
            return false;
        }

        return true;
    }
    
    std::expected<std::string_view, std::string> extract_ssid(
        const byte_span field
        , std::span<char> buffer
    ) {
        const auto ssid_len = field[1];
    
        if (buffer.size() < (ssid_len * 4L)) {
            return std::unexpected{"buffer too small or malformed field"};
        }

        auto ssid_bytes = field.subspan(IE_HEADER_LEN, ssid_len);

        auto *output = buffer.data();
        size_t i = 0;
        for (const auto& byte : ssid_bytes) {

            if (std::isprint(byte) == 0) {

                std::memcpy(output + i, "\\x", 2); // NOLINT(bugprone-not-null-terminated-result)
                i += 2;

                auto res = byte_to_str(output + i, byte);
                if (!res) {
                    return std::unexpected{res.error()};
                }

                i += res.value();
            }
            else {
                *(output + i) = byte; // NOLINT
                i++;
            }
        }

        return std::string_view{buffer.data(), i};
    }

    std::optional<byte_span> next_field(
        byte_span fields
        , size_t idx
    ) {
        if (idx + IE_HEADER_LEN + fields[idx + 1] <= fields.size()) {
            return fields.subspan(idx, IE_HEADER_LEN + fields[idx + 1]);
        }
    
        return {};
    }

}

std::expected<std::string_view, std::string> parse_ssid(
    const unsigned char *bss_info_elems
    , int bss_info_elems_len
    , std::span<char> buffer
) {
    // bss_info_elems: basic service set information elements
    // A collection of fields.
    // Each begins with a 2 bytes header (1 byte ID, 1 byte length)
    // followed by n bytes of data (as per the length).

    if (buffer.size() < SSID_BUF_MIN) {
        return std::unexpected{"buffer too small"};
    }

    const auto fields = byte_span(
        bss_info_elems
        , bss_info_elems_len
    );

    size_t idx = 0;

    while (auto field = next_field(fields, idx)) {
        if (is_valid_ssid(*field)) {
            return extract_ssid(*field, buffer);
        }
        idx += IE_HEADER_LEN + (*field)[1];
    }

    return std::unexpected{"no valid SSID found"};
}


}
