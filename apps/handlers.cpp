#include "handlers.hpp"
#include "get_nl80211_cmd.hpp"

#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <netlink/attr.h>
#include <netlink/errno.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>

#include <iostream>
#include <format>
#include <array>

namespace { 

    namespace ws = wifi_scanner;

}

int error_handler(sockaddr_nl * /*nla*/, nlmsgerr *err, void *arg) {
    std::cout << "error_handler() called.\n";
    int *ret = static_cast<int*>(arg);
    *ret = err->error;
    return NL_STOP;
}

int finish_handler(nl_msg * /*msg*/, void *arg) {
    std::cout << "finish_handler() called.\n";
    int *ret = static_cast<int*>(arg);
    *ret = 0;
    return NL_SKIP;
}


int ack_handler(nl_msg * /*msg*/, void *arg) {
    std::cout << "ack_handler() called.\n";
    auto *ret = static_cast<int*>(arg);
    *ret = 0;
    return NL_STOP;
}


int seq_handler(nl_msg *msg, void * /*arg*/) {
    nlmsghdr *nlh = nlmsg_hdr(msg);
    if (nlh != nullptr) {
        std::cout << "seq_handler() called. Sequence number: " << nlh->nlmsg_seq << "\n";
    } else {
        std::cout << "seq_handler() called. Failed to get message header.\n";
    }
    return NL_OK;
}

int validation_handler(nl_msg *msg, void *arg) {
    std::cout << "validation_callback() called.\n";

    // Get the "Generic Netlink Header"
    // https://www.infradead.org/~tgr/libnl/doc/api/group__msg.html#gae44a904bb40c8b5f5ff31539c21cfa5a
    void* res = nlmsg_data(nlmsg_hdr(msg));
    auto *gnlh = static_cast<genlmsghdr *>(res);

    auto *results = static_cast<TriggerScanResult *>(arg);

    auto cmd_str = ws::get_nl80211_cmd(static_cast<nl80211_commands>(gnlh->cmd));
    std::cout << std::format("Got command: {}.\n", cmd_str);

    if (gnlh->cmd == NL80211_CMD_SCAN_ABORTED) {
        results->done = true;
        results->aborted = true;
    } else if (gnlh->cmd == NL80211_CMD_NEW_SCAN_RESULTS) {
        results->done = true;
        results->aborted = false;
    }

    // Don't need to do this, just checking how it works.... Parse attributes
    std::array<nlattr *, NL80211_ATTR_MAX + 1> attrs{};

    // nlattr *attrs[NL80211_ATTR_MAX + 1];
    int const err = nla_parse(
        attrs.data()
        , NL80211_ATTR_MAX
        , genlmsg_attrdata(gnlh, 0)
        , genlmsg_attrlen(gnlh, 0)
        , nullptr
    );
    if (err < 0) {
        std::cerr << "Failed to parse attributes: " << nl_geterror(err) << "\n";
        return NL_SKIP;
    }

    // Access attributes (example)
    if (attrs[NL80211_ATTR_IFINDEX] != nullptr) {
        auto ifindex = nla_get_u32(attrs[NL80211_ATTR_IFINDEX]);
        std::cout << "Interface index: " << ifindex << "\n";
    }
    // if (attrs[NL80211_ATTR_IFNAME]) {
    //     std::cout << "Interface name: " << nla_get_string(attrs[NL80211_ATTR_IFNAME]) << "\n";
    // }

    return NL_SKIP;
}
