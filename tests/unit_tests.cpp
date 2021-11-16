#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <fmt/core.h>                          // for fmt::format

#include "utils.hpp"
#include "metaprogramming.hpp"

using tb::String;
using tb::trim;
using tb::meta::to_string;

TEST_CASE("Make file manager", "[file_manager]" ) {
    String file_name = "/home/typon/gitz/tabdeeli/tests/test_file.cpp";
    tb::FileManager fm = tb::read_file_into_file_manager(file_name);
    fmt::print("File manager: {}", to_string(fm));
    REQUIRE(fm.line_start_byte_indices.size() == 7);
}

TEST_CASE("Extract Lines from Byte Slice 0", "[file_manager]" ) {
    String file_name = "/home/typon/gitz/tabdeeli/tests/test_file.cpp";
    tb::FileManager fm = tb::read_file_into_file_manager(file_name);

    tb::U32 start, end;
    std::tie(start, end) = tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 24, .end = 46});
    REQUIRE(start == 2);
    REQUIRE(end == 3);

    std::tie(start, end) = tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 24, .end = 44});
    REQUIRE(start == 2);
    REQUIRE(end == 2);

    std::tie(start, end) = tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 24, .end = 45});
    REQUIRE(start == 2);
    REQUIRE(end == 3);

    std::tie(start, end) = tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 0});
    REQUIRE(start == 0);
    REQUIRE(end == 0);

    std::tie(start, end) = tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 1});
    REQUIRE(start == 0);
    REQUIRE(end == 0);

    std::tie(start, end) = tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 1000});
    REQUIRE(start == 0);
    REQUIRE(end == 6);

    std::tie(start, end) = tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 124});
    REQUIRE(start == 0);
    REQUIRE(end == 5);

    std::tie(start, end) = tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 125});
    REQUIRE(start == 0);
    REQUIRE(end == 6);

    std::tie(start, end) = tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 126});
    REQUIRE(start == 0);
    REQUIRE(end == 6);

    REQUIRE_THROWS(tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 1, .end = 0}));

    std::tie(start, end) = tb::get_surrounding_line_indices_for_byte_slice(fm, tb::ByteSlice {.start = 150, .end = 150});
    REQUIRE(start == 6);
    REQUIRE(end == 6);
}

TEST_CASE("Test json conversion", "[json]" ) {
    String x = "hello";
    tb::Foo f = {.bar = std::cref(x)};
    REQUIRE(trim(to_string(f)) == trim(R""""(
Foo {
    "bar": "hello"
})""""));
}
