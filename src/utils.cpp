#include <cassert>
#include <fstream>
#include <regex>
#include <fmt/core.h>                          // for fmt::format

#include "state.hpp"
#include "metaprogramming.hpp"

using tb::meta::to_string;

namespace tb
{

void
log(AppState* state, std::string_view msg)
{
    state->logger.print(msg);
    state->logger.print("\n");
    state->logger.flush();
}

void
log(Logger* logger, std::string_view msg)
{
    logger->print(msg);
    logger->print("\n");
    logger->flush();
}

Char**
vector_of_strings_to_double_char_array(const std::vector<String>& strings)
{
    Char** result = static_cast<Char**>(malloc(strings.size()));
    int index = 0;
    for (const String& string: strings)
    {
        result[index] = const_cast<Char*>(string.c_str());
    }
    return result;
}

std::string ltrim(const std::string &s) {
    return std::regex_replace(s, std::regex("^\\s+"), std::string(""));
}

std::string rtrim(const std::string &s) {
    return std::regex_replace(s, std::regex("\\s+$"), std::string(""));
}

std::string trim(const std::string &s) {
    return ltrim(rtrim(s));
}

FileManager
read_file_into_file_manager(const String& file_name)
{
    FileManager result = {
        .file_name = file_name,
        .contents = {},
        .line_start_byte_indices = {},
        /* .match_byte_slices = {}, */
    };

    /* U32 size = byte_end - byte_start + 1; */

    FILE* file_handle = std::fopen(file_name.c_str(), "rb");
    assert(file_handle != nullptr); // make sure the file is openable.

    /* std::fseek(file_handle, byte_start, SEEK_SET); // seek to start of slice */

    U32 byte_index = 0;
    Char prev_char = '\n';
    while (true)
    {
        Char curr_char = std::fgetc(file_handle);

        if (curr_char == EOF)
        {
            break;
        }

        if (prev_char == '\n')
        {
            result.line_start_byte_indices.push_back(byte_index);
        }

        result.contents.push_back(curr_char);

        byte_index++;
        prev_char = curr_char;
    }

    /* std::vector<U8> buffer(size); // create a buffer to read into */
    /* std::fread(buffer.data(), sizeof(U8), buffer.size(), file_handle); // read into it correct number of bytes */

    std::fclose(file_handle);

    // TODO: Replace String with a custom string class...
    // This currently causes an extra copy because STL containers must own their own memory.
    /* return String(buffer.begin(), buffer.end()); */
    return result;
}

std::string_view
get_line_starting_at_byte(const FileManager& file_manager, ByteSlice line_slice)
{
    return std::string_view(file_manager.contents.data() + line_slice.start, line_slice.end);
}

std::tuple<std::string_view, std::string_view>
get_surrounding_lines_for_byte_slice(const FileManager& file_manager, ByteSlice byte_slice)
{}

std::tuple<U32, U32>
get_surrounding_line_indices_for_byte_slice(const FileManager& file_manager, ByteSlice byte_slice)
{
    if (byte_slice.start > byte_slice.end)
    {
        throw std::runtime_error("byte slice ill-formed");
    }

    // [0, 10, 20, 30, 50, 52, 110]

    // (21, 31)
    auto start_line_iter =
        std::lower_bound(
            file_manager.line_start_byte_indices.begin(),
            file_manager.line_start_byte_indices.end(),
            byte_slice.start);
    auto end_line_iter =
        std::upper_bound(
            file_manager.line_start_byte_indices.begin(),
            file_manager.line_start_byte_indices.end(),
            byte_slice.end);

    S32 start_line_index = std::distance(file_manager.line_start_byte_indices.begin(), start_line_iter);
    S32 end_line_index = std::distance(file_manager.line_start_byte_indices.begin(), end_line_iter);

    if (start_line_index > 0)
    {
        // std::lower_bound returns the element AFTER the one in which the start_byte_index appears
        start_line_index = start_line_index - 1;
    }
    if (end_line_index > 0)
    {
        // std::upper_bound returns the element AFTER the one in which the end_byte_index appears
        end_line_index = end_line_index - 1;
    }

    /* fmt::print("\nline_start_byte: {}\n", to_string(file_manager.line_start_byte_indices)); */
    /* fmt::print("\nbyte_slice: {}, start: {}, end: {}\n", to_string(byte_slice), start_line_index, end_line_index); */

    return {start_line_index, end_line_index};
}

} // end of namespace
