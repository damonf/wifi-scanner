#include "nl_socket.hpp"
#include "nl_message.hpp"
#include "nl_error.hpp"

#include <libnl3/netlink/socket.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>

#include <stdexcept>
#include <string>
#include <expected>

namespace wifi_scanner {

NLSocket::NLSocket() 
    : sock_{nl_socket_alloc(), nl_socket_free} {

    if (!sock_) {
        throw std::runtime_error("failed to allocate netlink socket");
    }

	  auto res = genl_connect(sock_.get());
    if (res != 0) {
        throw std::runtime_error("failed to connect to generic netlink");
    }
}

std::expected<int, NLError> NLSocket::resolve_family(const std::string& name) const {
    auto res = genl_ctrl_resolve(sock_.get(), name.c_str());
    if (res < 0) {
        return std::unexpected{res};
    }
    return res;
}

std::expected<int, NLError> NLSocket::resolve_group(const std::string& family_name, const std::string& group_name) const {
    auto res = genl_ctrl_resolve_grp(sock_.get(), family_name.c_str(), group_name.c_str());
    if (res < 0) {
        return std::unexpected{res};
    }
    return res;
}

std::expected<void, NLError> NLSocket::add_membership(int group) {
    auto res = nl_socket_add_membership(sock_.get(), group);
    if (res < 0) {
        return std::unexpected{res};
    }
    return {};
}

std::expected<void, NLError> NLSocket::drop_membership(int group) {
    auto res = nl_socket_drop_membership(sock_.get(), group);
    if (res < 0) {
        return std::unexpected{res};
    }
    return {};
}

std::expected<void, NLError> NLSocket::modify_cb(nl_cb_type type, nl_cb_kind kind, nl_recvmsg_msg_cb_t func, void *arg) {
    auto res = nl_socket_modify_cb(sock_.get(), type, kind, func, arg);
    if (res < 0) {
        return std::unexpected{res};
    }
    return {};
}

std::expected<void, NLError> NLSocket::modify_err_cb(nl_cb_kind kind, nl_recvmsg_err_cb_t func, void *arg) {
    auto res = nl_socket_modify_err_cb(sock_.get(), kind, func, arg);
    if (res < 0) {
        return std::unexpected{res};
    }
    return {};
}

std::expected<int, NLError> NLSocket::send_auto(const NLMessage& msg) const {
    // https://www.infradead.org/~tgr/libnl/doc/api/group__send__recv.html#ga4ed435bdbaa2545641909ac9c39c48b0
    auto res = nl_send_auto(sock_.get(), msg.get());
    if (res < 0) {
        return std::unexpected{res};
    }
    return res;
}

std::expected<void, NLError> NLSocket::recvmsgs_default() const {
    auto res = nl_recvmsgs_default(sock_.get());
    if (res < 0) {
        return std::unexpected{res};
    }
    return {};
}

void NLSocket::disable_seq_check() const {
    nl_socket_disable_seq_check(sock_.get());
}

}
