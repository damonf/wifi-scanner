add_executable(wifi-scanner
  main.cpp
  handlers.hpp
  handlers.cpp
  )

target_link_libraries(wifi-scanner PRIVATE wifi-scanner-lib)
