#ifndef NL_ERROR_HPP
#define NL_ERROR_HPP

#include <netlink/genl/genl.h>

#include <string>

namespace wifi_scanner {

class NLError {
public:
    NLError(int res) {
        if (res < 0) {
            // nl_perror?
            error = nl_geterror(-res);
        }
    }
    NLError(const char* err) : error(err) {
    }

    [[nodiscard]] const std::string& str() const {
        return error;
    }
    [[nodiscard]] const char* c_str() const {
        return error.c_str();
    }

    operator const std::string&() const {
        return error;
    }

    operator const char*() const {
        return error.c_str();
    }

private:
    std::string error{}; // NOLINT
};

}

#endif // NL_ERROR_HPP
