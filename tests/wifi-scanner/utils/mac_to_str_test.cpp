#include <catch2/catch_test_macros.hpp>

#include "mac_to_str.hpp"

#include <array>

namespace wp = wifi_scanner::utils;

namespace wifi_scanner_tests {

TEST_CASE("mac to str test", "[mac_to_str]") {

    const std::array<unsigned char, 6> mac_addr = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

    SECTION("test convert mac address to string") {

        std::array<char, 18> buffer{};

        auto res = wp::mac_to_str(
            mac_addr.data(),
            mac_addr.size(),
            buffer
        );

        REQUIRE(res);

        auto mac_str = std::string{res.value()};

        REQUIRE(mac_str == "00:11:22:33:44:55");
    }

    SECTION("test invalid buffer size") {

        std::array<char, 16> buffer{};

        auto res = wp::mac_to_str(
            mac_addr.data(),
            mac_addr.size(),
            buffer
        );

        REQUIRE_FALSE(res);
    }
}

}


