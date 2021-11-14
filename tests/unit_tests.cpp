#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <fmt/core.h>                          // for fmt::format

#include "utils.hpp"
#include "metaprogramming.hpp"

using tb::String;
using tb::trim;
using tb::meta::to_string;

TEST_CASE("Extract Lines from Byte Slice", "[file_manager]" ) {
    tb::FileManager fm = tb::read_file_into_file_manager("/home/typon/gitz/tabdeeli/tests/test_file.cpp");
    fmt::print("File manager: {}", tb::meta::to_string(fm));
    REQUIRE(fm.line_start_byte_indices.size() == 7);
}

TEST_CASE("Test json conversion", "[json]" ) {
    String x = "hello";
    tb::Foo f = {.bar = std::cref(x)};
    REQUIRE(trim(to_string(f)) == trim(R""""(
Foo {
    "bar": "hello"
})""""));
}
