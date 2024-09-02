#ifndef PARSE_SSID_HPP
#define PARSE_SSID_HPP

#include <expected>
#include <span>
#include <string>
#include <string_view>

namespace wifi_scanner::parsers {

std::expected<std::string_view, std::string> parse_ssid(
    const unsigned char *bss_info_elems
    , int bss_info_elems_len
    , std::span<char> buffer
);

}

#endif // PARSE_SSID_HPP
