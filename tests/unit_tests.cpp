#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
/* #define CATCH_CONFIG_DISABLE_EXCEPTIONS */
#include "catch.hpp"

#include <fmt/core.h>                          // for fmt::format

#include "utils.hpp"
#include "searcher.hpp"
#include "metaprogramming.hpp"

using tb::String;
using tb::trim;
using tb::meta::to_string;

TEST_CASE("Make file manager", "[file_manager]" ) {
    String file_name = "/home/typon/gitz/tabdeeli/tests/test_file.cpp";
    tb::FileManager fm = tb::read_file_into_file_manager(file_name);
    REQUIRE(fm.line_start_byte_indices.size() == 8);
}

TEST_CASE("Extract Lines from Byte Slice 0", "[file_manager]" ) {
    String file_name = "/home/typon/gitz/tabdeeli/tests/test_file.cpp";
    tb::FileManager fm = tb::read_file_into_file_manager(file_name);

    tb::U32 start, end;
    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 24, .end = 46});
    REQUIRE(start == 2);
    REQUIRE(end == 3);

    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 24, .end = 44});
    REQUIRE(start == 2);
    REQUIRE(end == 2);

    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 24, .end = 45});
    REQUIRE(start == 2);
    REQUIRE(end == 3);

    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 0});
    REQUIRE(start == 0);
    REQUIRE(end == 0);

    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 1});
    REQUIRE(start == 0);
    REQUIRE(end == 0);

    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 1000});
    REQUIRE(start == 0);
    REQUIRE(end == 7);

    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 124});
    REQUIRE(start == 0);
    REQUIRE(end == 5);

    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 125});
    REQUIRE(start == 0);
    REQUIRE(end == 5);

    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 126});
    REQUIRE(start == 0);
    REQUIRE(end == 5);

    REQUIRE_THROWS(tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 1, .end = 0}));

    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 250, .end = 250});
    REQUIRE(start == 6);
    REQUIRE(end == 6);
}

TEST_CASE("Extract Lines from Byte Slice 1", "[file_manager]" ) {
    String file_name = "/home/typon/gitz/tabdeeli/tests/test_file2.cpp";
    tb::FileManager fm = tb::read_file_into_file_manager(file_name);

    tb::U32 start, end;
    std::tie(start, end) = tb::get_line_indices_spanning_byte_slice(fm, tb::ByteSlice {.start = 22, .end = 30});
    REQUIRE(start == 2);
    REQUIRE(end == 2);
}

TEST_CASE("Extract Lines from Byte Slice 2", "[file_manager]" ) {
    String file_name = "/home/typon/gitz/tabdeeli/tests/test_file.cpp";
    tb::FileManager fm = tb::read_file_into_file_manager(file_name);

    std::vector<tb::FileLine> lines = tb::get_lines_spanning_byte_slice(fm, tb::ByteSlice {.start = 24, .end = 46});
    REQUIRE(lines.size() == 2);
    REQUIRE(lines.at(0).content == "// Line 3 Blah blah blah");
    REQUIRE(lines.at(0).lineno == 2);
    REQUIRE(lines.at(1).content == "// Line 4");
    REQUIRE(lines.at(1).lineno == 3);

    lines = tb::get_lines_spanning_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 1});
    REQUIRE(lines.size() == 1);
    REQUIRE(lines.at(0).content == "// Line 1");
    REQUIRE(lines.at(0).lineno == 0);

    lines = tb::get_lines_spanning_byte_slice(fm, tb::ByteSlice {.start = 0, .end = 10});
    REQUIRE(lines.size() == 2);
    REQUIRE(lines.at(0).content == "// Line 1");
    REQUIRE(lines.at(0).lineno == 0);
    REQUIRE(lines.at(1).content == "// Line 2");
    REQUIRE(lines.at(1).lineno == 1);

}

TEST_CASE("Test column number", "[file_manager]" ) {
    String file_name = "/home/typon/gitz/tabdeeli/tests/test_file.cpp";
    tb::FileManager fm = tb::read_file_into_file_manager(file_name);

    std::vector<tb::FileLine> lines = tb::get_lines_spanning_byte_slice(fm, tb::ByteSlice {.start = 24, .end = 46});
    REQUIRE(lines.size() == 2);
    REQUIRE(lines.at(0).content == "// Line 3 Blah blah blah");
    REQUIRE(lines.at(0).lineno == 2);
    REQUIRE(lines.at(0).start_column == 4);
    REQUIRE(lines.at(1).content == "// Line 4");
    REQUIRE(lines.at(1).lineno == 3);

    lines = tb::get_lines_spanning_byte_slice(fm, tb::ByteSlice {.start = 3, .end = 5});
    REQUIRE(lines.size() == 1);
    REQUIRE(lines.at(0).content == "// Line 1");
    REQUIRE(lines.at(0).lineno == 0);
    REQUIRE(lines.at(0).start_column == 3);
}

TEST_CASE("Test json conversion", "[json]" ) {
    String x = "hello";
    tb::ByteSlice f = {.start = 0, .end = 1};
    REQUIRE(trim(to_string(f)) == trim(R""""(
ByteSlice {
    "end": 1,
    "start": 0
})""""));
}

TEST_CASE("Test search", "[search]" ) {
    tb::Logger logger = fmt::output_file("test.log");
    {
        tb::Searcher searcher;
        String s = "rew";
        String d = "/home/typon/gitz";
        tb::searcher::execute_search(&searcher, &logger, s, d);
        REQUIRE(searcher.num_results > 0);
    }

    {
        tb::Searcher searcher;
        String s = "lew";
        String d = "/home/typon/gitz/tabdeeli";
        tb::searcher::execute_search(&searcher, &logger, s, d);
        REQUIRE(searcher.num_results == 5);
    }
}

TEST_CASE("Test search 2", "[search]" ) {
    tb::Searcher searcher;
    tb::Logger logger = fmt::output_file("test.log");
    String s = "fruityloops";
    String d = "../";
    tb::searcher::execute_search(&searcher, &logger, s, d);
    REQUIRE(searcher.num_results > 0);
}

TEST_CASE("Test search 3", "[search]" ) {
    {
    tb::Searcher searcher;
    }
}
