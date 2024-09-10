#include "handlers.hpp"
#include "get_nl80211_cmd.hpp"
#include "nl_msg_parsers.hpp"
#include "bss_parsers.hpp"
#include "parse_ssid.hpp"

#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <netlink/attr.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>

#include <iostream>
#include <format>
#include <string>
#include <expected>

namespace { 

    namespace ws = wifi_scanner;
    namespace wp = wifi_scanner::parsers;

}

int error_handler(sockaddr_nl * /*nla*/, nlmsgerr *err, void * /*arg*/) {
    std::cout << "error_handler() called: " << err->error << "\n";
    return NL_STOP;
}

int finish_handler(nl_msg * /*msg*/, void * /*arg*/) {
    std::cout << "finish_handler() called.\n";
    return NL_SKIP;
}

int ack_handler(nl_msg * /*msg*/, void * /*arg*/) {
    std::cout << "ack_handler() called.\n";
    return NL_STOP;
}

int seq_handler(nl_msg *msg, void * /*arg*/) {
    std::cout << "seq_handler() called.";
    nlmsghdr *nlh = nlmsg_hdr(msg);
    if (nlh != nullptr) {
        std::cout << " Sequence number: " << nlh->nlmsg_seq;
    }
    std::cout << "\n";
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

    auto res = wp::parse_genlmsghdr(msg)
        .and_then(wp::parse_attribs)
        .and_then(wp::parse_bss_attribs)
        .and_then([](auto bss) {

            std::string buff(128, '\0');

            auto mac_addr = wp::parse_mac_addr(bss, buff);

            if (mac_addr) {
                std::cout << mac_addr.value() << ", ";
            }
            else {
                std::cerr << "failed to get mac address: " << mac_addr.error() << "\n";
            }

            auto freq = wp::parse_freq(bss);

            if (freq) {
                std::cout << std::to_string(freq.value()) << " MHz, ";
            }
            else {
                std::cerr << "failed to get frequency: " << freq.error() << "\n";
            }

            auto elems = wp::parse_bss_info_elems(bss);

            if (elems) {
                auto ssid = wp::parse_ssid(elems.value(), buff);
                
                if (ssid) {
                    std::cout << "\"" << ssid.value() << "\"";
                }
                else {
                    std::cerr << "failed to get SSID: " << ssid.error() << "\n";
                }
            }
            else {
                std::cerr << "failed to get bss info elems: " << elems.error() << "\n";
            }

            std::cout << "\n";
            return std::expected<void, std::string>{};
        })
        .or_else([](const std::string& err) {
            std::cerr << "failed to parse scan result: " << err << "\n";
            return std::expected<void, std::string>{};
        });

    return NL_SKIP;
}
