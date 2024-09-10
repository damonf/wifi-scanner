#include "handlers.hpp"
#include "nl_socket.hpp"
#include "nl_message.hpp"

#include <format>
#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <net/if.h>  // if_nametoindex 
#include <netlink/handlers.h>
#include <netlink/msg.h>

#include <array>
#include <expected>
#include <iostream>
#include <string>

#include <cstddef>
#include <cstdlib>
#include <cerrno>

namespace { 

    namespace ws = wifi_scanner;

}

constexpr size_t err_buf_size = 256;

std::string get_error(int errnum) {
    std::array<char, err_buf_size> buffer{};
    auto *msg = ::strerror_r(errnum, buffer.data(), sizeof(buffer)); // NOLINT(misc-include-cleaner)
    return msg;
}

std::expected<void, std::string> set_callbacks(ws::NLSocket& ucsk);
std::expected<void, std::string> trigger_scan(ws::NLSocket& ucsk, int fam, unsigned int if_index);
std::expected<void, std::string> get_scan_results(ws::NLSocket& ucsk, int fam, unsigned int if_index);
void clear_callbacks(ws::NLSocket& ucsk);

int main(int  argc, const char *argv[]) {
    if (argc < 2) {
        std::cerr << "must specify the wifi interface" << std::endl;
        return EXIT_FAILURE;
    }

    const char *ifname = argv[1];
    
    ws::NLSocket ucsk{};

    auto fam = ucsk.resolve_family("nl80211");
    if (!fam) {
        std::cerr << "failed to resolve family nl80211 with error: " << fam.error() << '\n';
        return EXIT_FAILURE;
    }

    auto if_index = if_nametoindex(ifname);
    if (if_index == 0) {
        std::cerr << "failed to resolve interface index with error: " << get_error(errno) << '\n';
        return EXIT_FAILURE;
    }

    auto res = set_callbacks(ucsk);
    if (!res) {
        std::cerr << "failed to set callbacks: " << res.error() << '\n';
        return EXIT_FAILURE;
    }

    res = trigger_scan(ucsk, fam.value(), if_index);
    if (!res) {
        std::cerr << "failed to trigger scan: " << res.error() << '\n';
        return EXIT_FAILURE;
    }

    res = get_scan_results(ucsk, fam.value(), if_index);
    if (!res) {
        std::cerr << "failed to get scan results: " << res.error() << '\n';
        return EXIT_FAILURE;
    }

    clear_callbacks(ucsk);

    return EXIT_SUCCESS;
}

std::expected<void, std::string> set_callbacks(ws::NLSocket& ucsk) {

    auto res = ucsk.modify_cb(
        NL_CB_ACK 
        , NL_CB_CUSTOM
        , ack_handler
        , nullptr
    );
    if (!res) {
        return std::unexpected{std::format("modify ack callback failed: {}", res.error().str())};
    }

    res = ucsk.modify_cb(
        NL_CB_SEQ_CHECK 
        , NL_CB_CUSTOM
        , seq_handler
        , nullptr
    );
    if (!res) {
        return std::unexpected{std::format("modify seq check callback failed: {}", res.error().str())};
    }

    res = ucsk.modify_cb(
        NL_CB_FINISH 
        , NL_CB_CUSTOM
        , finish_handler
        , nullptr
    );
    if (!res) {
        return std::unexpected{std::format("modify finish callback failed: {}", res.error().str())};
    }

    res = ucsk.modify_err_cb(
        NL_CB_CUSTOM
        , error_handler
        , nullptr
    );
    if (!res) {
        return std::unexpected{std::format("modify error callback failed: {}", res.error().str())};
    }

    return {};
}

std::expected<void, std::string> trigger_scan(ws::NLSocket& ucsk, int fam, unsigned int if_index) {

    auto mcgrp = ucsk.resolve_group("nl80211", "scan");
    if (!mcgrp) {
        return std::unexpected(std::format("failed to resolve group with error: {}", mcgrp.error().str()));
    }

    ws::NLSocket mcsk{};

    auto res = mcsk.add_membership(mcgrp.value());
    if (!res) {
        return std::unexpected(std::format("failed to add membership with error: {}", res.error().str()));
    }

    ws::NLMessage msg{};
    res = msg.put(
        fam
        , NL80211_CMD_TRIGGER_SCAN
        , NLM_F_REQUEST | NLM_F_ACK
        , 0
        , NL_AUTO_PORT
        , NL_AUTO_SEQ
    );

    if (!res) {
        return std::unexpected(std::format("failed to add Generic Netlink headers to Netlink message: {}", res.error().str()));
    }

    res = msg.put_u32(NL80211_ATTR_IFINDEX, if_index);

    if (!res) {
        return std::unexpected(std::format("failed to add 32 bit integer attribute to netlink message: {}", res.error().str()));
    }

    TriggerScanResult results{};

    res = mcsk.modify_cb(
        NL_CB_VALID 
        , NL_CB_CUSTOM
        , trigger_scan_handler
        , &results
    );
    if (!res) {
        return std::unexpected(std::format("modify validation callback failed: {}", res.error().str()));
    }

    mcsk.disable_seq_check();

    auto send_res = ucsk.send_auto(msg);
    if (!res) {
        return std::unexpected(std::format("unicast socket send_auto failed: {}", res.error().str()));
    }

    res = ucsk.recvmsgs_default();
    if (!res) {
        return std::unexpected(std::format("unicast socket recvmsgs_default failed: {}", res.error().str()));
    }

    while (!results.is_done()) {
        res = mcsk.recvmsgs_default();

        if (!res) {
            return std::unexpected(std::format("multicast socket recvmsgs_default failed: {}", res.error().str()));
        }
    }

    if (results.is_aborted()) {
        return std::unexpected("kernel aborted scan.");
    }

    return {};
}

std::expected<void, std::string> get_scan_results(ws::NLSocket& ucsk, int fam, unsigned int if_index) {

    ws::NLMessage msg{};
    auto res = msg.put(
        fam
        , NL80211_CMD_GET_SCAN 
        , NLM_F_REQUEST | NLM_F_ACK | NLM_F_DUMP 
        , 0
        , NL_AUTO_PORT
        , NL_AUTO_SEQ
    );

    if (!res) {
        return std::unexpected(std::format("failed to add Generic Netlink headers to Netlink message: {}", res.error().str()));
    }

    res = msg.put_u32(NL80211_ATTR_IFINDEX, if_index);

    if (!res) {
        return std::unexpected(std::format("failed to add 32 bit integer attribute to netlink message: {}", res.error().str()));
    }

    res = ucsk.modify_cb(
        NL_CB_VALID 
        , NL_CB_CUSTOM
        , get_scan_handler
        , nullptr
    );
    if (!res) {
        return std::unexpected(std::format("modify validation callback failed: {}", res.error().str()));
    }

    auto send_res = ucsk.send_auto(msg);
    if (!send_res) {
        return std::unexpected(std::format("send failed: {}", res.error().str()));
    }

    res = ucsk.recvmsgs_default();
    if (!res) {
        return std::unexpected(std::format("recvmsgs failed: {}", res.error().str()));
    }

    return {};
}

void clear_callbacks(ws::NLSocket& ucsk) {
    auto res = ucsk.clear_cb(NL_CB_ACK, NL_CB_CUSTOM);
    res = ucsk.clear_cb(NL_CB_SEQ_CHECK, NL_CB_CUSTOM);
    res = ucsk.clear_cb(NL_CB_FINISH, NL_CB_CUSTOM);
    res = ucsk.clear_err_cb(NL_CB_CUSTOM);
}
