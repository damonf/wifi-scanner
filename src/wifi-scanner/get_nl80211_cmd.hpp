#ifndef GET_NL80211_CMD_HPP
#define GET_NL80211_CMD_HPP

#include <linux/nl80211.h>

#include <string>

namespace wifi_scanner {

std::string get_nl80211_cmd(nl80211_commands cmd);

}

#endif // GET_NL80211_CMD_HPP
