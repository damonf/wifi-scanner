#ifndef NL_MESSAGE_HPP
#define NL_MESSAGE_HPP

#include "nl_error.hpp"

#include <netlink/attr.h>
#include <netlink/msg.h>

#include <memory>
#include <expected>

namespace wifi_scanner {

using nl_msg_ptr = std::unique_ptr<nl_msg, decltype(&nlmsg_free)>;

class NLMessage {

public:
    NLMessage();

    [[nodiscard]] std::expected<void, NLError> put(
        int fam
        , uint8_t cmd
        , int flags = 0
        , int hdrlen = 0
        , uint32_t port = NL_AUTO_PORT
        , uint32_t seq = NL_AUTO_SEQ
        , uint8_t version = 0
    );

    [[nodiscard]] std::expected<void, NLError> put_u32(int attrtype, uint32_t value);

    [[nodiscard]] nl_msg* get() const {
        return msg_.get();
    }

private:
    nl_msg_ptr msg_;
};

}

#endif // NL_MESSAGE_HPP 
