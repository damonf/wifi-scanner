#ifndef MAC_TO_STR_HPP
#define MAC_TO_STR_HPP

#include <expected>
#include <span>
#include <string>

namespace wifi_scanner::utils {

std::expected<std::string_view, std::string> mac_to_str(
    const unsigned char *mac_addr
    , int mac_addr_len
    , std::span<char> buffer
);

}

#endif // MAC_TO_STR_HPP
