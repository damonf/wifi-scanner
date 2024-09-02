#ifndef BYTE_TO_STR_HPP
#define BYTE_TO_STR_HPP

#include <string>
#include <expected>
#include <cstddef>

namespace wifi_scanner::utils {

std::expected<size_t, std::string> byte_to_str(char *output, const unsigned char &byte);

}

#endif // BYTE_TO_STR_HPP
