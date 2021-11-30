#pragma once

#include <optional>
#include <algorithm>
#include <fn.hpp>

#include "ftxui/dom/elements.hpp"  // for text, separator, bold, hcenter, vbox, hbox, gauge, Element, operator|, border

#include "components.hpp"
#include "state.hpp"

namespace fn = rangeless::fn;
using fn::operators::operator%;

namespace tb
{
void log(AppState* state, std::string_view msg);
void log(Logger* logger, std::string_view msg);
Char** vector_of_strings_to_double_char_array(const std::vector<String>& strings);
FileManager read_file_into_file_manager(const String& file_name);
std::vector<U32> get_line_start_byte_indices_for_file(const String& file_contents);
FileManager replace_text_in_file(const FileManager& file_manager, ByteSlice byte_slice, String replacement_text);
std::vector<FileLine> get_lines_spanning_byte_slice(const FileManager&, ByteSlice);
std::vector<FileLine> create_lines_with_replaced_text(const std::vector<FileLine>& prev_lines, const String& replacement_text);
std::tuple<U32, U32> get_line_indices_spanning_byte_slice(const FileManager&, ByteSlice);
std::string trim(const std::string &s);
std::string rtrim(const std::string &s);
std::string ltrim(const std::string &s);
std::vector<std::string> split_string(const std::string& s, U32 every_n_chars);
std::vector<U32> split_term_x_into_three_by_ratios(U32 total_width, F32 first, F32 second, F32 third);
}

namespace tb::functional
{

static auto keys = [](const auto& map)
{
    return
        fn::refs(map)
        % fn::transform([](const auto& pair) {
            return std::cref(pair.get().first);
        });
};

static auto sorted_keys = [](const auto& map)
{
    return
        keys(map)
        % fn::lazy_sort()
        % fn::unique_adjacent()
        % fn::to_vector();
};

static auto string_to_bytes = [](const auto& string)
{
    return
        fn::from(string.begin(), string.end())
        % fn::transform([&](const auto& character) {
            return U8(character);
        })
        % fn::to_vector();
};

static auto bytes_to_uints = [](const auto& bytes)
{
    return
        fn::from(bytes.begin(), bytes.end())
        % fn::transform([&](const auto& byte) {
            return U32(byte);
        })
        % fn::to_vector();
};

inline std::string truncate_string(const std::string& str, U64 max_len)
{
    U64 len = str.length();
    if (len > max_len)
    {
        U64 diff = len - max_len;
        return "..." + str.substr(diff + 4, len);
    }
    return str;
}

static auto truncate_strings = [](const auto& strings, U64 max_len)
{
    return
        fn::from(strings.begin(), strings.end())
        % fn::transform([&](const auto& string) {
            return truncate_string(string, max_len);
        })
        % fn::to_vector();
};

static auto get_from_map = [](const auto& map, const auto& key)
{
    const auto& item_iterator = map.find(key);
    if (item_iterator == map.end())
    {
        return std::nullopt;
    }
    else
    {
        return std::make_optional(std::cref(item_iterator->second));
    }
};

// Returns an optional over the value stored in the map
static auto get_from_map_by_value = [](const auto& map, const auto& key)
    -> std::optional<typename std::remove_reference<decltype(map)>::type::mapped_type>
{
    const auto& item_iterator = map.find(key);
    if (item_iterator == map.end())
    {
        return std::nullopt;
    }
    else
    {
        return std::make_optional(item_iterator->second);
    }
};

static auto text_views_from_file_lines = [](const auto& file_lines, const auto& line_color, const auto& line_prefix)
{
    using namespace ftxui;

    auto terminal_x_size = Terminal::Size().dimx;
    U32 max_flexible_paragraph_line_width = U32(terminal_x_size / 2.5);

    return
        fn::refs(file_lines)
        % fn::transform([&](const FileLine& file_line) {
            S32 tmp_lineno = file_line.lineno;
            S32 num_digits_in_lineno = 0; do { tmp_lineno /= 10; num_digits_in_lineno++; } while (tmp_lineno != 0);
            S32 gutter_width = std::max(num_digits_in_lineno, 4);

            return hbox({
                color(line_color, text(line_prefix)),
                color(Color::GrayLight, text(fmt::format("{:>{}}", file_line.lineno + 1, gutter_width))) | bold | size(WIDTH, EQUAL, gutter_width),
                color(Color::GrayLight, text("| ")),
                color(line_color, hflow(ftxui_extras::flexible_paragraph(file_line.content, max_flexible_paragraph_line_width)))
            }) | yflex_grow;
        })
        % fn::to_vector();
};

} // end namespace tb::functional
