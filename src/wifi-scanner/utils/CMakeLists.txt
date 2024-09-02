target_sources(wifi-scanner-lib
    PRIVATE
        byte_to_str.hpp
        byte_to_str.cpp
        mac_to_str.hpp
        mac_to_str.cpp
)

target_include_directories(wifi-scanner-lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
