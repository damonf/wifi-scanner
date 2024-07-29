#ifndef NL_SOCKET_HPP
#define NL_SOCKET_HPP

#include "nl_error.hpp"
#include "nl_message.hpp"

#include <libnl3/netlink/handlers.h>
#include <netlink/netlink.h>

#include <memory>
#include <expected>

namespace wifi_scanner {

using nl_sock_ptr = std::unique_ptr<nl_sock, decltype(&nl_socket_free)>;

class NLSocket {

public:
    NLSocket();
    
    [[nodiscard]] std::expected<int, NLError> resolve_family(const std::string& name) const;
    [[nodiscard]] std::expected<int, NLError> resolve_group(const std::string& family_name, const std::string& group_name) const;
    [[nodiscard]] std::expected<void, NLError> add_membership(int group);
    [[nodiscard]] std::expected<void, NLError> drop_membership(int group);
    [[nodiscard]] std::expected<void, NLError> modify_cb(nl_cb_type type, nl_cb_kind kind, nl_recvmsg_msg_cb_t func, void *arg);
    [[nodiscard]] std::expected<void, NLError> modify_err_cb(nl_cb_kind kind, nl_recvmsg_err_cb_t func, void *arg);
    [[nodiscard]] std::expected<void, NLError> send_auto(const NLMessage& msg) const;
    [[nodiscard]] std::expected<void, NLError> recvmsgs_default() const;
    void disable_seq_check() const;

private:
    nl_sock_ptr sock_;
};

}

#endif // NL_SOCKET_HPP 
