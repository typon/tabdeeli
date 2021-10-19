cmake_minimum_required (VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)

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
  GIT_TAG d141cdbeb0fb422a3fb7173b285fd38e0d1772dc
)

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
  FetchContent_Populate(fmt)
  add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------

project(ftxui-starter
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(ftxui-starter src/main.cpp)
target_include_directories(ftxui-starter PRIVATE src)

target_link_libraries(ftxui-starter
  PRIVATE fmt::fmt
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
  PUBLIC stdc++
)

install(TARGETS ftxui-starter RUNTIME DESTINATION "bin")
