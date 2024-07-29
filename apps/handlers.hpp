#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <linux/netlink.h>
#include <netlink/attr.h>

struct TriggerScanResult {
    bool done{};
    bool aborted{};
};

int error_handler(sockaddr_nl *nla, nlmsgerr *err, void *arg);
int finish_handler(nl_msg *msg, void *arg);
int ack_handler(nl_msg *msg, void *arg);
int seq_handler(nl_msg *msg, void *arg);
int validation_handler(nl_msg *msg, void *arg);

#endif // HANDLERS_HPP
