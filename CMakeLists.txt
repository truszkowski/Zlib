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
