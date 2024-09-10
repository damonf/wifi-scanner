#include "nl_msg_parsers.hpp"

#include <linux/genetlink.h>
#include <netlink/attr.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <linux/nl80211.h>
#include <netlink/errno.h>
#include <netlink/genl/genl.h>

#include <expected>
#include <string>
#include <vector>
#include <format>
#include <array>

namespace wifi_scanner::parsers {

std::expected<genlmsghdr*, std::string> parse_genlmsghdr(nl_msg *msg) {

    if (msg == nullptr) {
        return std::unexpected{"ml_msg is null"};
    }

    // https://www.infradead.org/~tgr/libnl/doc/api/group__msg.html#gae44a904bb40c8b5f5ff31539c21cfa5a
    void *res = nlmsg_data(nlmsg_hdr(msg));
    return static_cast<genlmsghdr *>(res);
}

std::expected<std::vector<nlattr*>, std::string> parse_attribs(genlmsghdr *gnlh) {
    std::vector<nlattr*> tb{NL80211_ATTR_MAX + 1};

    // https://www.infradead.org/~tgr/libnl/doc/api/group__attr.html#gaa7ad544b5a93034602a442eb26cda92c
    auto err = nla_parse(
        tb.data()
        , NL80211_ATTR_MAX
        , genlmsg_attrdata(gnlh, 0)
        , genlmsg_attrlen(gnlh, 0)
        , nullptr
    );

    if (err != 0) {
        return std::unexpected{std::format("failed to parse attributes: {}", nl_geterror(err))};
    }

    return tb;
}

std::expected<std::vector<nlattr*>, std::string> parse_bss_attribs(const std::vector<nlattr*>& tb) {

    if (tb[NL80211_ATTR_BSS] == nullptr) {
        return std::unexpected{"NL80211_ATTR_BSS not found"};
    }

    // The policy defines the expected types for the attributes.
    // Type Mismatch: Parsing may proceed, but the resulting data may be incorrect or invalid.
    // Missing Attributes: The corresponding entries in the parsed attribute array will be NULL. You must handle these cases in your code.
    // Extra Attributes: These will be ignored and will not affect the parsing of defined attributes.
    // https://www.infradead.org/~tgr/libnl/doc/core.html#core_attr_parse
    const static constinit std::array<nla_policy, NL80211_BSS_MAX + 1> bss_policy = [] {

        std::array<nla_policy, NL80211_BSS_MAX + 1> res{};
        res[NL80211_BSS_TSF]                  = nla_policy{NLA_U64, 0, 0};
        res[NL80211_BSS_TSF]                  = nla_policy{NLA_U32, 0, 0};
        res[NL80211_BSS_BSSID]                = nla_policy{};
        res[NL80211_BSS_BEACON_INTERVAL]      = nla_policy{NLA_U16, 0, 0};
        res[NL80211_BSS_CAPABILITY]           = nla_policy{NLA_U16, 0, 0};
        res[NL80211_BSS_INFORMATION_ELEMENTS] = nla_policy{};
        res[NL80211_BSS_SIGNAL_MBM]           = nla_policy{NLA_U32, 0, 0};
        res[NL80211_BSS_SIGNAL_UNSPEC]        = nla_policy{NLA_U8, 0, 0};
        res[NL80211_BSS_STATUS]               = nla_policy{NLA_U32, 0, 0};
        res[NL80211_BSS_SEEN_MS_AGO]          = nla_policy{NLA_U32, 0, 0};
        res[NL80211_BSS_BEACON_IES]           = nla_policy{};

        return res;
    }();

    std::vector<nlattr*> bss{NL80211_BSS_MAX + 1};

    // https://www.infradead.org/~tgr/libnl/doc/api/group__attr.html#ga974ec0720b2d863a5c65da868bcb7a6e
    auto err = nla_parse_nested(
            bss.data()
            , NL80211_BSS_MAX
            , tb[NL80211_ATTR_BSS]
            , bss_policy.data()
        );

    if (err != 0) {
        return std::unexpected{std::format("Failed to parse bss attributes: {}", nl_geterror(err))};
    }

    return bss;
}

}
