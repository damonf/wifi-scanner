#include "nl_message.hpp"
#include "nl_error.hpp"

#include <netlink/genl/genl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <stdexcept>
#include <cstdint>
#include <expected>

namespace wifi_scanner {

NLMessage::NLMessage() 
    : msg_{nlmsg_alloc(), nlmsg_free} {

    if (!msg_) {
        throw std::runtime_error("failed to allocate netlink message");
    }
}

std::expected<void, NLError> NLMessage::put(
    int fam
    , uint8_t cmd
    , int flags
    , int hdrlen
    , uint32_t port
    , uint32_t seq
    , uint8_t version
) {

    // https://www.infradead.org/~tgr/libnl/doc/api/group__genl.html#ga9a86a71bbba6961d41b8a75f62f9e946
    auto *hdr = genlmsg_put(
        msg_.get()
        , port
        , seq
        , fam
        , hdrlen
        , flags
        , cmd
        , version
    );

    if (hdr == nullptr) {
        return std::unexpected{"genlmsg_put failed to alloc"};
    }

    return {};
}

std::expected<void, NLError> NLMessage::put_u32(int attrtype, uint32_t value) {

    // https://www.infradead.org/~tgr/libnl/doc/api/group__attr.html#ga67adaa9c52579b96883a7c3520ad6643 
    auto res = nla_put_u32(
        msg_.get()
       , attrtype
       , value
    );

    if (res < 0) {
        return std::unexpected{res};
    }

    return {};
}

}
