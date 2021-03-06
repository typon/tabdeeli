# Copyright 2021 Davidson Francis <davidsondfgl@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.5.2)
set(CMAKE_C_STANDARD 99)

project(ag C)
include(GNUInstallDirs)

# General
add_compile_options(-Wall -Wextra)
include_directories(${CMAKE_SOURCE_DIR})
add_definitions(-D_GNU_SOURCE)

# Files
set(AG_SRC
	ag_src/decompress.c
	ag_src/ignore.c
	ag_src/lang.c
	ag_src/log.c
	ag_src/main.c
	ag_src/options.c
	ag_src/print.c
	ag_src/print_w32.c
	ag_src/scandir.c
	ag_src/search.c
	ag_src/util.c
	ag_src/zfile.c
)
set(LIBAG_DOC
	doc/man3/ag_finish.3
	doc/man3/ag_free_all_results.3
	doc/man3/ag_free_result.3
	doc/man3/ag_get_stats.3
	doc/man3/ag_init.3
	doc/man3/ag_init_config.3
	doc/man3/ag_search.3
	doc/man3/ag_search_ts.3
	doc/man3/ag_set_config.3
	doc/man3/ag_start_workers.3
	doc/man3/ag_stop_workers.3
)

# libag objects
add_library(libag_objects OBJECT ${AG_SRC} libag.c)
set_target_properties(libag_objects PROPERTIES
	PUBLIC_HEADER libag.h
	POSITION_INDEPENDENT_CODE 1
)
target_include_directories(libag_objects PRIVATE ag_src/)

# libag
add_library(ag SHARED $<TARGET_OBJECTS:libag_objects>)
target_link_libraries(ag pcre lzma z pthread)

# pkg-config
configure_file(doc/libag.pc.in libag.pc @ONLY)

# Install libag
install(TARGETS ag
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# Install manpages
install(FILES doc/man1/ag.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES ${LIBAG_DOC} DESTINATION ${CMAKE_INSTALL_MANDIR}/man3)
# Install pkgconfig
install(FILES ${CMAKE_BINARY_DIR}/libag.pc
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
