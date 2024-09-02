#ifndef BSS_PARSER_HPP
#define BSS_PARSER_HPP

#include <cstdint>
#include <linux/netlink.h>

#include <vector>
#include <string>
#include <expected>

namespace wifi_scanner::parsers {

class BssParser {
public:
    BssParser(const std::vector<nlattr*>& bss);

    [[nodiscard]] std::expected<std::string, std::string> mac_addr() const {
        return mac_addr_;
    }
    
    [[nodiscard]] std::expected<std::string, std::string> ssid() const {
        return ssid_;
    }
    
    [[nodiscard]] std::expected<uint32_t, std::string> freq() const {
        return freq_;
    }

private:

    static std::expected<std::string, std::string> extract_mac_addr(
            const std::vector<nlattr*>& bss
            , std::string& buff
        );

    static std::expected<uint32_t, std::string> extract_freq(
        const std::vector<nlattr*>& bss
    );

    static std::expected<std::string, std::string> extract_ssid(
        const std::vector<nlattr*>& bss
        , std::string& buff
    );

    std::expected<std::string, std::string> mac_addr_;
    std::expected<std::string, std::string> ssid_;
    std::expected<uint32_t, std::string> freq_;
};

}

#endif // BSS_PARSER_HPP
