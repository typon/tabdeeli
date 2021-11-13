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

add_subdirectory(external_libs/libag)

# ------------------------------------------------------------------------------
# These external dependencies could not be fetched using github directly. (above method)
include_directories(external_libs/libag)
include_directories(external_libs/rangeless/include)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Dependencies used only for development and testing
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
FetchContent_Declare(rttr
  GIT_REPOSITORY https://github.com/rttrorg/rttr
  GIT_TAG e338d9aa47f6c7da97be88df3aaa48ac9c5fe6c6
)

FetchContent_GetProperties(rttr)
if(NOT rttr_POPULATED)
  FetchContent_Populate(rttr)
  add_subdirectory(${rttr_SOURCE_DIR} ${rttr_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG 7194245a314d142a5560f6906a87f7c67ebbcf2e
)

FetchContent_GetProperties(nlohmann_json)
if(NOT nlohmann_json_POPULATED)
  FetchContent_Populate(nlohmann_json)
  add_subdirectory(${nlohmann_json_SOURCE_DIR} ${nlohmann_json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------
include_directories(external_libs/catch2)
# ------------------------------------------------------------------------------

# End of dependencies used only for development and testing
# ------------------------------------------------------------------------------

project(tabdeeli
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(tabdeeli
    src/main.cpp
    src/components.cpp
    src/searcher.cpp
    src/object_utils.cpp
    src/utils.cpp
    src/styled_button.cpp
    src/serde.cpp
    src/metaprogramming.cpp
    src/metaprogramming.hpp
)

add_executable(tabdeeli_unit_tests
    tests/unit_tests.cpp
    src/utils.cpp
)

target_include_directories(tabdeeli PRIVATE src)
target_include_directories(tabdeeli_unit_tests PRIVATE src)

target_link_libraries(tabdeeli
  PRIVATE ag
  PRIVATE fmt::fmt
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
  PRIVATE RTTR
  PRIVATE nlohmann_json::nlohmann_json
  PUBLIC stdc++
)

target_link_libraries(tabdeeli_unit_tests
  PRIVATE ag
  PRIVATE fmt::fmt
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
  PRIVATE RTTR
  PRIVATE nlohmann_json::nlohmann_json
  PUBLIC stdc++
)

install(TARGETS tabdeeli RUNTIME DESTINATION "bin")
