#include <cassert>
#include <fstream>

#include "state.hpp"

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

std::tuple<String, String>
get_surrounding_lines_for_byte_slice(const FileManager& file_manager, ByteSlice byte_slice)
{

    U32 left_idx = 0;
    U32 right_idx = file_manager.line_start_byte_indices.size() - 1;

    // [0, 10, 20, 30, 50, 52, 110]

    // (21, 31)
    auto start_line_iter = std::lower_bound(file_manager.line_start_byte_indices.begin(), file_manager.line_start_byte_indices.end(), byte_slice.start);
    auto end_line_iter = std::upper_bound(file_manager.line_start_byte_indices.begin(), file_manager.line_start_byte_indices.end(), byte_slice.end);

    // Binary search for line(s) which contain this given byte slice
/*     while (true) */
/*     { */
/*         U32 mid_idx = (left_idx + right_idx) / 2; */
/*         U32 line_start_byte = file_manager.line_start_byte_indices.at(mid_idx); */
/*         // This line start byte is too low */
/*         // This means the starting line must be found between left_idx and mid_idx - 1 */
/*         if (line_start_byte <= byte_slice.start) */
/*         { */
/*             right_idx = mid_idx - 1; */
/*         } */
/*         else if (line_start_byte <= byte_slice.start) */
/*         { */
/*         } */
/*  */
/*  */
/*     } */


}

} // end of namespace
