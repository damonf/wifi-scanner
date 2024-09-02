#include "bss_parser.hpp"
#include "mac_to_str.hpp"
#include "parse_ssid.hpp"

#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <netlink/attr.h>

#include <format>
#include <string>
#include <vector>
#include <expected>
#include <cstdint>

namespace wifi_scanner::parsers {

BssParser::BssParser(const std::vector<nlattr*>& bss) {

    std::string buff(128, '\0');

    mac_addr_ = extract_mac_addr(bss, buff);
    freq_ = extract_freq(bss);
    ssid_ = extract_ssid(bss, buff);
}

std::expected<std::string, std::string> BssParser::extract_mac_addr(
        const std::vector<nlattr*>& bss
        , std::string& buff
    ) {

    nlattr* attr = bss[NL80211_BSS_BSSID];

    if (attr == nullptr) {
        return std::unexpected{"mac address not present"};
    }

    void *data = nla_data(attr);

    auto mac_str = utils::mac_to_str(
        static_cast<unsigned char*>(data)
        , nla_len(attr)
        , buff
    );

    if (!mac_str) {
        return std::unexpected{
            std::format("failed to convert mac address to string: {}\n"
            , mac_str.error())
        };
    }

    return std::string{mac_str.value()};
}

std::expected<uint32_t, std::string> BssParser::extract_freq(
        const std::vector<nlattr*>& bss
    ) {

    nlattr* attr = bss[NL80211_BSS_FREQUENCY];

    if (attr != nullptr) {
        return nla_get_u32(attr);
    }

    return std::unexpected{"frequency not present"};
}

std::expected<std::string, std::string> BssParser::extract_ssid(
        const std::vector<nlattr*>& bss
        , std::string& buff
    ) {

    nlattr* attr = bss[NL80211_BSS_INFORMATION_ELEMENTS];

    if (attr == nullptr) {
        return std::unexpected{"SSID not present"};
    }

    void *data = nla_data(attr);

    auto ssid = parse_ssid(
        static_cast<unsigned char*>(data)
        , nla_len(attr)
        , buff
    );

    if (!ssid) {
        return std::unexpected{
            std::format("failed to convert ssid to string: {}\n"
            , ssid.error())
        };
    }

    return std::string{ssid.value()};
}

}
