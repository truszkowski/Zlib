# -----------------------------------------------------------------------------
#
#            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
#                    Version 2, December 2004
#  
# Copyright (C) 2004 Sam Hocevar
#  14 rue de Plaisance, 75014 Paris, France
# Everyone is permitted to copy and distribute verbatim or modified
# copies of this license document, and changing it is allowed as long
# as the name is changed.
# 
#            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
#   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
# 
#  0. You just DO WHAT THE FUCK YOU WANT TO.
#
# -----------------------------------------------------------------------------

cmake_minimum_required (VERSION 2.8) 
project (Zlib)

set (CMAKE_CXX_FLAGS "-ggdb3 -Wall -Wextra -Werror -O2")
set (VERSION "0.2.0")

include_directories (BEFORE ${CMAKE_SOURCE_DIR})

add_library (Zlib Zlib.cpp)

add_executable (Zinf Zinf.cpp)
add_executable (Zdef Zdef.cpp)
add_executable (Zgetline Zgetline.cpp)
add_executable (Ztextgen Ztextgen.cpp)
add_executable (Zfilegen Zfilegen.cpp)

target_link_libraries (Zinf Zlib z)
target_link_libraries (Zdef Zlib z)
target_link_libraries (Zgetline Zlib z)

enable_testing ()
add_test (testZ ${CMAKE_SOURCE_DIR}/testit)

install (FILES Zlib.h DESTINATION include/)
install (TARGETS Zlib DESTINATION lib/)
install (TARGETS Zinf Zdef Zgetline DESTINATION bin/)
include (CPack)
