target_sources(test-wifi-scanner
    PRIVATE
)

target_include_directories(test-wifi-scanner PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(utils)
add_subdirectory(parsers)
