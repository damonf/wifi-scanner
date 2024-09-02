target_sources(wifi-scanner-lib
    PRIVATE
    nl_msg_parsers.hpp
    nl_msg_parsers.cpp
    parse_ssid.hpp
    parse_ssid.cpp
    bss_parser.hpp
    bss_parser.cpp
)

target_include_directories(wifi-scanner-lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
