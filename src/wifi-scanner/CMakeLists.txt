target_sources(wifi-scanner-lib
  PRIVATE
  nl_socket.hpp
  nl_socket.cpp
  nl_message.hpp
  nl_message.cpp
  nl_error.hpp
  get_nl80211_cmd.hpp
  get_nl80211_cmd.cpp
  )

target_include_directories(wifi-scanner-lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(parsers)
add_subdirectory(utils)
