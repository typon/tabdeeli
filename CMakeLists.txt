cmake_minimum_required (VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
# --- Fetch FTXUI --------------------------------------------------------------

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG 75cd2b0fca6baa6c59c8ef9187275b1de342e29a
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 1266c2b6003e6391046bbab57dcf20293e25dedd
)

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
  FetchContent_Populate(fmt)
  add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ag
  GIT_REPOSITORY https://github.com/typon/libag
  GIT_TAG 3d00785278e039ae0df2b06f10facea2bf6cf9c4
)

FetchContent_GetProperties(ag)
if(NOT ag_POPULATED)
  FetchContent_Populate(ag)
  add_subdirectory(${ag_SOURCE_DIR} ${ag_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(external_libs/libag)

include_directories(external_libs/rangeless/include)
# ------------------------------------------------------------------------------

project(tabdeeli
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(tabdeeli
    src/main.cpp
    src/components.cpp
    src/searcher.cpp
    src/utils.cpp
    src/styled_button.cpp
)

target_include_directories(tabdeeli PRIVATE src)

target_link_libraries(tabdeeli
  PRIVATE ag
  PRIVATE fmt::fmt
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
  PUBLIC stdc++
)

install(TARGETS tabdeeli RUNTIME DESTINATION "bin")
