cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    TapReader
    VERSION 1.0
    DESCRIPTION "wifi scanner"
    LANGUAGES CXX
    )

# generate the test target
enable_testing()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# add_compile_options(
#   -D__cpp_lib_expected=202211 -Wno-builtin-macro-redefined
# )
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b")

add_compile_options(
  -Wall
  -Wextra
  -Wwrite-strings
  -Wno-parentheses
  -Wno-comment
  -Wpedantic
  -Warray-bounds
  -Weffc++
  # -Werror
  )

# run clang-tidy
set(CMAKE_CXX_CLANG_TIDY 
  clang-tidy;
  -header-filter=${CMAKE_CURRENT_SOURCE_DIR};
  #-warnings-as-errors=*;

  -extra-arg=-std=c++23;
  -extra-arg=-D__cpp_concepts=202002;
)

add_subdirectory(apps)
add_subdirectory(src)
add_subdirectory(tests)

message(STATUS "_GNU_SOURCE=${_GNU_SOURCE}")
