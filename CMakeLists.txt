cmake_minimum_required (VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")


set(BUILD_BENCHMARKS OFF CACHE INTERNAL "Turn off")
set(BUILD_EXAMPLES OFF CACHE INTERNAL "Turn off")
set(BUILD_UNIT_TESTS OFF CACHE INTERNAL "Turn off")
set(BUILD_DOCUMENTATION OFF CACHE INTERNAL "Turn off")
set(FTXUI_BUILD_EXAMPLES OFF CACHE INTERNAL "Turn off")
set(FTXUI_BUILD_DOCS OFF CACHE INTERNAL "Turn off")

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
# --- Fetch FTXUI --------------------------------------------------------------

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG 4188ee2c046ced00cd3accba4cde2318587f8c8a
)

# ------------------------------------------------------------------------------

add_subdirectory(external_libs/libag)

# ------------------------------------------------------------------------------
# These external dependencies could not be fetched using github directly. (above method)
include_directories(external_libs/libag)
include_directories(external_libs/rangeless/include)
include_directories(external_libs/clipp/include)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Dependencies used only for development and testing
# ------------------------------------------------------------------------------

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 1266c2b6003e6391046bbab57dcf20293e25dedd
)

# ------------------------------------------------------------------------------

FetchContent_Declare(RTTR
  GIT_REPOSITORY https://github.com/rttrorg/rttr
  GIT_TAG e338d9aa47f6c7da97be88df3aaa48ac9c5fe6c6
)

# ------------------------------------------------------------------------------

FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG 7194245a314d142a5560f6906a87f7c67ebbcf2e
)

# ------------------------------------------------------------------------------
include_directories(external_libs/catch2)
# ------------------------------------------------------------------------------

# End of dependencies used only for development and testing
# ------------------------------------------------------------------------------

# Cmake will download all the required deps and install them.
FetchContent_MakeAvailable(ftxui fmt RTTR nlohmann_json)

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
    src/metaprogramming.cpp
    src/flexible_paragraph.cpp
    src/flexible_menu.cpp
    src/flexible_container.cpp
    src/flexible_input.cpp
)

add_executable(tabdeeli_unit_tests
    tests/unit_tests.cpp
    src/utils.cpp
    src/metaprogramming.cpp
    src/searcher.cpp
)

target_include_directories(tabdeeli PRIVATE src)
target_include_directories(tabdeeli_unit_tests PRIVATE src)

target_link_libraries(tabdeeli
  PRIVATE ag
  PRIVATE fmt::fmt
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
  PRIVATE RTTR::Core
  PRIVATE nlohmann_json::nlohmann_json
  PUBLIC stdc++
)

target_link_libraries(tabdeeli_unit_tests
  PRIVATE ag
  PRIVATE fmt::fmt
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
  PRIVATE RTTR::Core
  PRIVATE nlohmann_json::nlohmann_json
  PUBLIC stdc++
)

install(TARGETS tabdeeli RUNTIME DESTINATION "bin")
