#ifndef BSS_PARSER_HPP
#define BSS_PARSER_HPP

#include <cstdint>
#include <linux/netlink.h>

#include <span>
#include <vector>
#include <string>
#include <expected>

namespace wifi_scanner::parsers {

std::expected<std::string_view, std::string> parse_mac_addr(
        const std::vector<nlattr*>& bss
        , std::string& buff
    );

std::expected<uint32_t, std::string> parse_freq(
    const std::vector<nlattr*>& bss
);

std::expected<std::span<const unsigned char>, std::string> parse_bss_info_elems(
    const std::vector<nlattr*>& bss
);


}

#endif // BSS_PARSER_HPP
