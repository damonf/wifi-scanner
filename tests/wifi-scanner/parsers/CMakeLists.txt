target_sources(test-wifi-scanner
    PRIVATE
    parse_ssid_test.cpp
)

target_include_directories(test-wifi-scanner PUBLIC ${CMAKE_CURRENT_LIST_DIR})


