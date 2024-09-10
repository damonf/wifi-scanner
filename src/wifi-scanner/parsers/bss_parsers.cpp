#include "bss_parsers.hpp"
#include "mac_to_str.hpp"

#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <netlink/attr.h>

#include <format>
#include <string>
#include <vector>
#include <expected>
#include <cstdint>
#include <span>
#include <string_view>

namespace wifi_scanner::parsers {

using cbyte_span = std::span<const unsigned char>;

std::expected<std::string_view, std::string> parse_mac_addr(
        const std::vector<nlattr*>& bss
        , std::string& buff
    ) {

    nlattr* attr = bss[NL80211_BSS_BSSID];

    if (attr == nullptr) {
        return std::unexpected{"mac address not present"};
    }

    void *data = nla_data(attr);

    return utils::mac_to_str(
        static_cast<unsigned char*>(data)
        , nla_len(attr)
        , buff
    )
    .or_else([](const std::string& err) -> std::expected<std::string_view, std::string> {
        return std::unexpected{
            std::format("failed to convert mac address to string: {}\n"
            , err)
        };
    });
}

std::expected<uint32_t, std::string> parse_freq(
        const std::vector<nlattr*>& bss
    ) {

    nlattr* attr = bss[NL80211_BSS_FREQUENCY];

    if (attr != nullptr) {
        return nla_get_u32(attr);
    }

    return std::unexpected{"frequency not present"};
}

std::expected<cbyte_span, std::string> parse_bss_info_elems(
        const std::vector<nlattr*>& bss
    ) {

    nlattr* attr = bss[NL80211_BSS_INFORMATION_ELEMENTS];

    if (attr == nullptr) {
        return std::unexpected{"SSID not present"};
    }

    void *data = nla_data(attr);

    return cbyte_span(
        static_cast<const unsigned char*>(data)
        , nla_len(attr)
    );
}

}
