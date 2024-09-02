#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_message.hpp>

#include "parse_ssid.hpp"

#include <array>
#include <algorithm>
#include <string>

#include <cstddef>

// NOLINTBEGIN(readability-function-cognitive-complexity)

namespace wp = wifi_scanner::parsers;

namespace wifi_scanner_tests {

TEST_CASE("parse ssid test", "[parse_ssid]") {

    const static constinit size_t ie_header_len = 2;
    static constexpr std::string ssid = "my test wifi";
    const static constinit size_t ssid_field_len = ie_header_len + ssid.size();
    const static size_t BUFFER_SIZE = 128;

    SECTION("test convert ssid to string") {

        std::array<unsigned char, ssid_field_len> bss_info_elems = {};

        // First byte is ID 0, which indicates the SSID
        bss_info_elems[0] = 0;
        // Second byte set to the length of the remaining bytes
        bss_info_elems[1] = ssid.size();

        // Copy the SSID string into the remaining bytes of the array
        std::copy(ssid.begin(), ssid.end(), bss_info_elems.begin() + ie_header_len);

        const unsigned char * const ie_ptr = bss_info_elems.data();

        // dest buffer
        std::array<char, BUFFER_SIZE> buffer{};

        auto res = wp::parse_ssid(
            ie_ptr
            , bss_info_elems.size()
            , buffer
        );

        if (!res) {
            CAPTURE(res.error());
            REQUIRE(false);
        }

        REQUIRE(res);

        auto ssid_str = std::string{res.value()};

        REQUIRE(ssid_str == "my test wifi");
    }

    SECTION("test convert ssid to string with multiple fields") {

        const static size_t first_field_len = ie_header_len + 5;
        const static constinit size_t ie_len = first_field_len + ssid_field_len;

        std::array<unsigned char, ie_len> bss_info_elems = {};

        // First field has ID 1, which indicates it's NOT a SSID
        bss_info_elems[0] = 1;
        // Second byte set to the length of the remaining bytes
        bss_info_elems[1] = 5;

        // First byte is ID 0, which indicates the SSID
        bss_info_elems[first_field_len] = 0;
        // Second byte set to the length of the remaining bytes
        bss_info_elems[first_field_len + 1] = ssid.size();

        // Copy the SSID string into the remaining bytes of the array
        std::copy(ssid.begin(), ssid.end(), bss_info_elems.begin() + first_field_len + ie_header_len);

        const unsigned char * const ie_ptr = bss_info_elems.data();

        std::array<char, BUFFER_SIZE> buffer{};

        auto res = wp::parse_ssid(
            ie_ptr
            , bss_info_elems.size()
            , buffer
        );

        if (!res) {
            CAPTURE(res.error());
            REQUIRE(false);
        }

        REQUIRE(res);

        auto ssid_str = std::string{res.value()};

        REQUIRE(ssid_str == "my test wifi");
    }

    SECTION("test convert ssid to string with buffer too small") {

        std::array<unsigned char, ssid_field_len> bss_info_elems = {};

        // First byte is ID 0, which indicates the SSID
        bss_info_elems[0] = 0;
        // Second byte set to the length of the remaining bytes
        bss_info_elems[1] = ssid.size();

        // Copy the SSID string into the remaining bytes of the array
        std::copy(ssid.begin(), ssid.end(), bss_info_elems.begin() + ie_header_len);

        const unsigned char * const ie_ptr = bss_info_elems.data();

        // dest buffer is too small
        std::array<char, 1> buffer{};

        auto res = wp::parse_ssid(
            ie_ptr
            , bss_info_elems.size()
            , buffer
        );

        REQUIRE(!res);
        REQUIRE(res.error() == "buffer too small");
    }

    SECTION("test convert ssid to string with no fields") {

        // too small to contain any fields
        std::array<unsigned char, 1> bss_info_elems = {};

        const unsigned char * const ie_ptr = bss_info_elems.data();

        std::array<char, BUFFER_SIZE> buffer{};

        auto res = wp::parse_ssid(
            ie_ptr
            , bss_info_elems.size()
            , buffer
        );

        REQUIRE(!res);
        REQUIRE(res.error() == "no valid SSID found");
    }

    SECTION("test convert ssid to string with no valid SSID") {

        const static size_t field_len = ie_header_len + 5;

        std::array<unsigned char, field_len> bss_info_elems = {};

        // First field has ID 1, which indicates it's NOT a SSID
        bss_info_elems[0] = 1;
        // Second byte set to the length of the remaining bytes
        bss_info_elems[1] = 5;

        const unsigned char * const ie_ptr = bss_info_elems.data();

        std::array<char, BUFFER_SIZE> buffer{};

        auto res = wp::parse_ssid(
            ie_ptr
            , bss_info_elems.size()
            , buffer
        );

        REQUIRE(!res);
        REQUIRE(res.error() == "no valid SSID found");
    }
}

// NOLINTEND(readability-function-cognitive-complexity)
}


