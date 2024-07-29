find_package(Catch2 3 REQUIRED
  PATHS ${PROJECT_SOURCE_DIR}/third_party/bin/Catch2 NO_DEFAULT_PATH
  )

include_directories(
  SYSTEM
  "${PROJECT_SOURCE_DIR}/third_party/bin/Catch2/include"
)

add_executable(test-wifi-scanner)

add_subdirectory(wifi-scanner)

target_link_libraries(test-wifi-scanner
  PRIVATE
  wifi-scanner-lib
  Catch2::Catch2WithMain
  # trompeloeil::trompeloeil
  )

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

# Run the function defined by the Catch2 CMake module
catch_discover_tests(test-wifi-scanner)
