#include <cassert>
#include <fstream>
#include <regex>
#include <fmt/core.h>                          // for fmt::format
#include <string>

#include "state.hpp"
#include "metaprogramming.hpp"

namespace tb
{

void
log(AppState* state, std::string_view msg)
{
    state->logger.print("{}", msg);
    state->logger.print("\n");
    state->logger.flush();
}

void
log(Logger* logger, std::string_view msg)
{
    logger->print("{}", msg);
    logger->print("\n");
    logger->flush();
}

Char**
vector_of_strings_to_double_char_array(const std::vector<String>& strings)
{
    Char** result = static_cast<Char**>(malloc(sizeof(Char*)*strings.size()));
    int index = 0;
    for (const String& string: strings)
    {
        result[index] = const_cast<Char*>(string.c_str());
        index++;
    }
    return result;
}

std::string
ltrim(const std::string &s) {
    return std::regex_replace(s, std::regex("^\\s+"), std::string(""));
}

std::string
rtrim(const std::string &s) {
    return std::regex_replace(s, std::regex("\\s+$"), std::string(""));
}

std::string
trim(const std::string &s) {
    return ltrim(rtrim(s));
}

std::vector<std::string>
split_string(const std::string& s, U32 every_n_chars)
{
   U32 num_sub_strings = s.length() / every_n_chars;

   std::vector<std::string> result;
   for (U32 i = 0; i < num_sub_strings; i++)
   {
        result.push_back(s.substr(i * every_n_chars, every_n_chars));
   }

   // If there are leftover characters, create a shorter item at the end.
   if (s.length() % every_n_chars != 0)
   {
        result.push_back(s.substr(every_n_chars * num_sub_strings));
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
    };

    FILE* file_handle = std::fopen(file_name.c_str(), "rb");
    assert(file_handle != nullptr); // make sure the file is openable.

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

    std::fclose(file_handle);

    // TODO: Replace String with a custom string class...
    // This currently causes an extra copy because STL containers must own their own memory.
    /* return String(buffer.begin(), buffer.end()); */
    return result;
}

B32
write_file_to_disk(const String& file_name, const String& contents)
{
    FILE* file_handle = std::fopen(file_name.c_str(), "w");
    if (file_handle != nullptr)
    {
        fprintf(file_handle, contents.c_str());
        fclose(file_handle);
        return true;
    }
    return false;
}

std::vector<U32>
get_line_start_byte_indices_for_file(const String& file_contents)
{
    U32 byte_index = 0;
    Char prev_char = '\n';

    std::vector<U32> result;
    while (true)
    {
        if (byte_index == file_contents.length())
        {
            break;
        }

        Char curr_char = file_contents.at(byte_index);

        if (prev_char == '\n')
        {
            result.push_back(byte_index);
        }

        byte_index++;
        prev_char = curr_char;
    }

    return result;
}

std::string_view
get_line_starting_at_byte(const FileManager& file_manager, ByteSlice line_slice)
{
    return std::string_view(file_manager.contents.data() + line_slice.start, line_slice.end);
}

std::tuple<U32, U32>
get_line_indices_spanning_byte_slice(const FileManager& file_manager, ByteSlice byte_slice)
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

    if (
        start_line_index >= file_manager.line_start_byte_indices.size() // start_byte was outside the file
        or byte_slice.start < file_manager.line_start_byte_indices.at(start_line_index))
    {
        // std::lower_bound returns the element AFTER the one in which the start_byte_index appears
        // However, if the start_byte happens to be the SAME as the line start, then we need to not decrement
        // because lower_bound gives the element thats strictly GREATER, not EQUAL
        start_line_index = start_line_index - 1;
    }
    if (end_line_index > 0)
    {
        // std::upper_bound returns the element AFTER the one in which the end_byte_index appears
        end_line_index = end_line_index - 1;
    }

    return {start_line_index, end_line_index};
}

std::vector<FileLine>
get_lines_spanning_byte_slice(const FileManager& file_manager, ByteSlice byte_slice)
{
    const auto& [start_line, end_line] = get_line_indices_spanning_byte_slice(file_manager, byte_slice);

    std::vector<FileLine> result;
    for (U32 lineno = start_line; lineno <= end_line; lineno++)
    {
        U64 start_byte = file_manager.line_start_byte_indices.at(lineno);
        U64 end_byte =
            (lineno == file_manager.line_start_byte_indices.size() - 1) ?
                file_manager.contents.size() - 1 :
                file_manager.line_start_byte_indices.at(lineno + 1) - 1; // - 1 to discount \n
        U64 num_bytes = end_byte - start_byte;

        U64 start_column = -1;
        if (lineno == start_line)
        {
            start_column = byte_slice.start - start_byte;
            start_column = start_column;
        }


        result.push_back(FileLine {
            .content = file_manager.contents.substr(start_byte, num_bytes),
            .lineno = lineno,
            .start_column = start_column,
        });
    }
    return result;
}

FileManager
replace_text_in_file(const FileManager& file_manager, ByteSlice byte_slice, String replacement_text)
{
    FileManager result = file_manager;

    U32 size = byte_slice.end - byte_slice.start + 1;
    result.contents.replace(byte_slice.start, size, replacement_text);

    result.line_start_byte_indices = get_line_start_byte_indices_for_file(result.contents);

    return result;
}

std::vector<U32>
split_term_x_into_three_by_ratios(U32 total_width, F32 first, F32 second, F32 third)
{
    return {
        U32(first * total_width),
        U32(second * total_width),
        U32(third * total_width),
    };
}

ftxui::Color
c(std::tuple<U32, U32, U32> color_tuple)
{
    return ftxui::Color::RGB(
        std::get<0>(color_tuple),
        std::get<1>(color_tuple),
        std::get<2>(color_tuple)
    );
}

} // end of namespace
