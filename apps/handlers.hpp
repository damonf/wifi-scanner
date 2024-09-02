#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <cstdint>
#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <netlink/attr.h>

class TriggerScanResult {
public:
    void update(uint8_t cmd) {
        if (cmd == NL80211_CMD_SCAN_ABORTED) {
            done = true;
            aborted = true;
        } else if (cmd == NL80211_CMD_NEW_SCAN_RESULTS) {
            done = true;
            aborted = false;
        }
    }
    [[nodiscard]] bool is_done() const {
        return done;
    }

    [[nodiscard]] bool is_aborted() const {
        return aborted;
    }

private:
    bool done{};
    bool aborted{};
};

int error_handler(sockaddr_nl *nla, nlmsgerr *err, void *arg);
int finish_handler(nl_msg *msg, void *arg);
int ack_handler(nl_msg *msg, void *arg);
int seq_handler(nl_msg *msg, void *arg);
int trigger_scan_handler(nl_msg *msg, void *arg);
int get_scan_handler(nl_msg *msg, void *arg);

#endif // HANDLERS_HPP
