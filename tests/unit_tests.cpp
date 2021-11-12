#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "utils.hpp"

TEST_CASE( "Extract Lines from Byte Slice", "[file_manager]" ) {

    tb::FileManager fm = tb::read_file_into_file_manager("/home/typon/gitz/tabdeeli/tests/test_file.cpp");
    REQUIRE(fm.line_start_byte_indices.size() == 7);

}
