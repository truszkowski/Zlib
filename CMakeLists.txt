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
project (zlibpp)

if (DEFINED ENV{VIRTUAL_ENV})
	message (STATUS "Detected active virtualenv: $ENV{VIRTUAL_ENV}")
	include (venv/pkg)
else ()
	message (STATUS "Not detected active virtualenv")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra -g -pg -pipe -O2")
	set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}")
	set (CMAKE_VERBOSE_MAKEFILE   off)
	set (CMAKE_COLOR_MAKEFILE     on)
	set (CMAKE_USE_RELATIVE_PATHS on)
endif ()
	
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

if (DEFINED ENV{VIRTUAL_ENV})
	include (CPack)
endif ()
