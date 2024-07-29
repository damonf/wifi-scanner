#include "handlers.hpp"
#include "nl_socket.hpp"
#include "nl_message.hpp"

#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <net/if.h>  // if_nametoindex 
#include <netlink/handlers.h>
#include <netlink/msg.h>

#include <array>
#include <exception>
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

int main(int  /*argc*/, const char * /*argv*/[]) {


    // const std::expected<int, char> abc = 42;
    //
    // #if defined(__cplusplus)
    //     std::cout << "__cplusplus is: " << __cplusplus << std::endl;
    // #endif
    //
    // #if defined(__cpp_concepts)
    //     std::cout << "__cpp_concepts is:" << __cpp_concepts << std::endl;
    // #endif
    //
    // #if defined(__cpp_lib_expected)
    //     std::cout << "__cpp_lib_expected is: " << __cpp_lib_expected << std::endl;
    //
    //     const std::expected<int, char> e = 42;
    // #else
    //     std::cout << "std::expected is not available." << std::endl;
    // #endif

    try {
        ws::NLSocket ucsk{};
        ws::NLSocket mcsk{};

        auto fam = ucsk.resolve_family("nl80211");
        if (!fam) {
            std::cerr << "failed to resolve family nl80211 with error: " << fam.error() << '\n';
            return EXIT_FAILURE;
        }

        auto if_index = if_nametoindex("wlp0s20f3"); // TODO: cmd line arg 
        if (if_index == 0) {
            std::cerr << "failed to resolve interface index with error: " << get_error(errno) << '\n';
            return EXIT_FAILURE;
        }
        
        // Trigger Scan ....

        auto mcgrp = ucsk.resolve_group("nl80211", "scan");
        if (!mcgrp) {
            std::cerr << "failed to resolve group with error: " << mcgrp.error() << '\n';
            return EXIT_FAILURE;
        }

        auto res = mcsk.add_membership(mcgrp.value());
        if (!res) {
            std::cerr << "failed to add membership with error: " << res.error() << '\n';
            return EXIT_FAILURE;
        }

        // Create the message ...

        ws::NLMessage msg{};
        res = msg.put(
            fam.value()
            , NL80211_CMD_TRIGGER_SCAN
            , NLM_F_REQUEST | NLM_F_ACK
            , 0
            , NL_AUTO_PORT
            , NL_AUTO_SEQ
        );

        if (!res) {
            std::cerr << "failed to add Generic Netlink headers to Netlink message: "
                << res.error() << '\n';
            return EXIT_FAILURE;
        }

        res = msg.put_u32(NL80211_ATTR_IFINDEX, if_index);

        if (!res) {
            std::cerr << "failed to add 32 bit integer attribute to netlink message: "
                << res.error() << '\n';
            return EXIT_FAILURE;
        }

        // Set up the callbacks ...

        int err = 1;
        res = ucsk.modify_cb(
           NL_CB_ACK 
         , NL_CB_CUSTOM
         , ack_handler
         , &err
        );
        if (!res) {
            std::cerr << "modify ack callback failed: " << res.error() << '\n';
            return EXIT_FAILURE;
        }

        res = ucsk.modify_cb(
           NL_CB_SEQ_CHECK 
         , NL_CB_CUSTOM
         , seq_handler
         , &err
        );
        if (!res) {
            std::cerr << "modify seq check callback failed: " << res.error() << '\n';
            return EXIT_FAILURE;
        }

        res = ucsk.modify_cb(
           NL_CB_FINISH 
         , NL_CB_CUSTOM
         , finish_handler
         , &err
        );
        if (!res) {
            std::cerr << "modify finish callback failed: " << res.error() << '\n';
            return EXIT_FAILURE;
        }

        res = ucsk.modify_err_cb(
           NL_CB_CUSTOM
         , error_handler
         , &err
        );
        if (!res) {
            std::cerr << "modify error callback failed: " << res.error() << '\n';
            return EXIT_FAILURE;
        }

        TriggerScanResult results{};

        res = mcsk.modify_cb(
           NL_CB_VALID 
         , NL_CB_CUSTOM
         , validation_handler
         , &results
        );
        if (!res) {
            std::cerr << "modify validation callback failed: " << res.error() << '\n';
            return EXIT_FAILURE;
        }

        mcsk.disable_seq_check();

        // Send the message on the unicast socket
        res = ucsk.send_auto(msg);
        if (!res) {
            std::cerr << "unicast socket send_auto failed: " << res.error() << '\n';
            return EXIT_FAILURE;
        }

        res = ucsk.recvmsgs_default();
        if (!res) {
            std::cerr << "unicast socket recvmsgs_default failed: " << res.error() << '\n';
            return EXIT_FAILURE;
        }

        if (err < 0) {
            std::cerr << "error occurred: " << err << '\n';
            err = 1;
            // TODO: exit now?
        }
 
        while (!results.done) {
            res = mcsk.recvmsgs_default();

            if (!res) {
                std::cerr << "multicast socket recvmsgs_default failed: " << res.error() << '\n';
                return EXIT_FAILURE;
            }
        }

        if (results.aborted) {
            std::cerr << "kernel aborted scan.\n";
            return EXIT_FAILURE;
        }

        std::cout << "scan is done.\n";

        res = ucsk.modify_cb(NL_CB_ACK, NL_CB_CUSTOM, nullptr, nullptr);
        res = ucsk.modify_cb(NL_CB_SEQ_CHECK, NL_CB_CUSTOM, nullptr, nullptr);
        res = ucsk.modify_cb(NL_CB_FINISH, NL_CB_CUSTOM, nullptr, nullptr);
        res = ucsk.modify_err_cb(NL_CB_CUSTOM, nullptr, nullptr);
        res = mcsk.modify_cb(NL_CB_VALID, NL_CB_CUSTOM, nullptr, nullptr);

        res = mcsk.drop_membership(mcgrp.value());
        
        // TODO: get scan results
 

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
}

