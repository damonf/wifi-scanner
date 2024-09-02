#include "handlers.hpp"
#include "get_nl80211_cmd.hpp"
#include "nl_msg_parsers.hpp"
#include "bss_parser.hpp"

#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <netlink/attr.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>

#include <iostream>
#include <format>
#include <string>

namespace { 

    namespace ws = wifi_scanner;
    namespace wp = wifi_scanner::parsers;

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

int trigger_scan_handler(nl_msg *msg, void *arg) {
    std::cout << "trigger_scan_handler() called.\n";

    auto gen_hdr = wp::parse_genlmsghdr(msg);

    if (!gen_hdr) {
        std::cerr << "failed to create GenNlMsgHeader: " << gen_hdr.error() << "\n";
        return NL_SKIP;
    }

    auto *results = static_cast<TriggerScanResult *>(arg);
    auto cmd = gen_hdr.value()->cmd;
    auto cmd_str = ws::get_nl80211_cmd(static_cast<nl80211_commands>(cmd));
    std::cout << std::format("Got command: {}.\n", cmd_str);

    results->update(cmd);

    // Parse attributes
    // auto tp = wp::parse_attribs(gen_hdr.value());
    // if (!tp) {
    //     std::cerr << "failed to parse attributes: " << tp.error() << "\n";
    //     return NL_SKIP;
    // }
    // auto attrs = tp.value();
    //
    // if (attrs[NL80211_ATTR_IFINDEX] != nullptr) {
    //     auto ifindex = nla_get_u32(attrs[NL80211_ATTR_IFINDEX]);
    //     std::cout << "Interface index: " << ifindex << "\n";
    // }

    return NL_SKIP;
}

int get_scan_handler(nl_msg *msg, [[maybe_unused]] void *arg) {

    auto gen_hdr = wp::parse_genlmsghdr(msg);

    if (!gen_hdr) {
        std::cerr << "failed to create GenNlMsgHeader: " << gen_hdr.error() << "\n";
        return NL_SKIP;
    }
    
    auto tp = wp::parse_attribs(gen_hdr.value());

    if (!tp) {
        std::cerr << "failed to parse attributes: " << tp.error() << "\n";
        return NL_SKIP;
    }

    auto bss = wp::parse_bss_attribs(tp.value());

    if (!bss) {
        std::cerr << "failed to parse BSS attributes: " << bss.error() << "\n";
        return NL_SKIP;
    }

    const wp::BssParser bss_parser{bss.value()};
    auto mac_addr = bss_parser.mac_addr();
    auto freq = bss_parser.freq();
    auto ssid = bss_parser.ssid();

    std::cout << (mac_addr ? mac_addr.value() : "???") << ", ";
    std::cout << (freq ? std::to_string(freq.value()) : "???") << " MHz, ";
    std::cout << "\"" << (ssid ? ssid.value() : "???") << "\"";
    std::cout << "\n";

    if (!mac_addr) {
        std::cerr << "failed to get mac address: " << mac_addr.error() << "\n";
    }
    if (!freq) {
        std::cerr << "failed to get frequency: " << freq.error() << "\n";
    }
    if (!ssid) {
        std::cerr << "failed to get SSID: " << ssid.error() << "\n";
    }

    return NL_SKIP;
}
