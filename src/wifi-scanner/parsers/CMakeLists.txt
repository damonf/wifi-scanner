target_sources(wifi-scanner-lib
    PRIVATE
    nl_msg_parsers.hpp
    nl_msg_parsers.cpp
    parse_ssid.hpp
    parse_ssid.cpp
    bss_parsers.hpp
    bss_parsers.cpp
)

target_include_directories(wifi-scanner-lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
