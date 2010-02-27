cmake_minimum_required (VERSION 2.6) 
if (COMMAND cmake_policy)
  cmake_policy (SET CMP0003 NEW)
endif (COMMAND cmake_policy)


project (Zlib)


set (CMAKE_CXX_FLAGS "-ggdb3 -Wall -Wextra -Werror -O2")

add_library (Zlib Zlib.cpp)

add_executable (Zinf Zinf.cpp)
add_executable (Zdef Zdef.cpp)
add_executable (Zgetline Zgetline.cpp)
add_executable (Ztextgen Ztextgen.cpp)
add_executable (Zfilegen Zfilegen.cpp)

target_link_libraries (Zinf Zlib z)
target_link_libraries (Zdef Zlib z)
target_link_libraries (Zgetline Zlib z)
