add_library(wifi-scanner-lib "")
add_subdirectory(wifi-scanner)

# https://www.infradead.org/~tgr/libnl/doc/core.html#_linking_to_this_library
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNL REQUIRED libnl-3.0 libnl-genl-3.0)

target_link_libraries(wifi-scanner-lib PRIVATE ${LIBNL_LIBRARIES})
target_include_directories(wifi-scanner-lib SYSTEM PUBLIC ${LIBNL_INCLUDE_DIRS})

# function(print_all_variables)
#   message(STATUS "Printing all CMake variables:")
#   get_cmake_property(_variableNames VARIABLES)
#   foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName} = [${${_variableName}}]")
#   endforeach()
# endfunction()
#
# print_all_variables()

message(STATUS "LIBNL_INCLUDE_DIRS=${LIBNL_INCLUDE_DIRS}")
message(STATUS "LIBNL_LIBRARIES=${LIBNL_LIBRARIES}")
