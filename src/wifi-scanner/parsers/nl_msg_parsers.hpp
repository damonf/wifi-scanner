#ifndef PARSE_GENLMSGHDR_HPP
#define PARSE_GENLMSGHDR_HPP 

#include <linux/genetlink.h>
#include <linux/nl80211.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/genl/genl.h>

#include <expected>
#include <string>
#include <vector>

namespace wifi_scanner::parsers {

[[nodiscard]] std::expected<genlmsghdr*, std::string> parse_genlmsghdr(nl_msg* msg);
[[nodiscard]] std::expected<std::vector<nlattr*>, std::string> parse_attribs(genlmsghdr* gnlh);
[[nodiscard]] std::expected<std::vector<nlattr*>, std::string> parse_bss_attribs(const std::vector<nlattr*>& tb);

}

#endif // PARSE_GENLMSGHDR_HPP 
