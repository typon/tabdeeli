#pragma once

#include <optional>
#include <algorithm>
#include <set>
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
B32 write_file_to_disk(const String& file_name, const String& contents);
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

static auto verify_byte_slices = [] (const auto& byte_slices)
{
    std::move(byte_slices)
    % fn::sliding_window(2)
    % fn::for_each([](const auto& prev_next_view) {
        const auto& prev_next = prev_next_view % fn::to_vector();
        const auto& prev = prev_next.at(0);
        const auto& next = prev_next.at(1);
        assert(prev.byte_slice.end < next.byte_slice.start);
    });
};

static auto commit_diffs_to_disk = [] (AppState* app_state)
{
    std::set<String> success_files;
    std::set<String> error_files;

    fn::refs(app_state->history_viewer_state.diffs)
    % fn::where([](const TextDiff& diff) {
        return diff.accepted == true;
    })
    % fn::group_all_by([](const TextDiff& diff) {
        return diff.file_name;
    })
    % fn::for_each([&](const auto& diffs_for_file) {
        std::vector<TextDiff> sorted_diffs =
            fn::refs(diffs_for_file)
            % fn::sort_by([](const TextDiff& diff) {
                return diff.byte_slice.start;
            })
            % fn::transform([](const auto& diff) { return diff.get(); })
            % fn::to_vector();

        U32 file_index = sorted_diffs.at(0).file_index;
        const FileManager& original_file_manager = app_state->file_picker_state.file_managers.at(file_index);
        const String& original_file_contents = original_file_manager.contents;
        const String& original_file_name = original_file_manager.file_name;
        String new_file_contents;
        String new_file_name = original_file_name;
        auto file_cursor = original_file_contents.begin();

        for (const TextDiff& diff: sorted_diffs)
        {
            auto num_prev_bytes = std::distance(file_cursor, original_file_contents.begin() + diff.byte_slice.start);
            new_file_contents.append(file_cursor, file_cursor + num_prev_bytes);

            std::advance(file_cursor, num_prev_bytes);

            new_file_contents.append(diff.replacement_text);

            std::advance(file_cursor, (diff.byte_slice.end - diff.byte_slice.start) + 1);
        }
        // Copy everything after the last diff
        const TextDiff& last_diff = sorted_diffs.at(sorted_diffs.size() - 1);
        new_file_contents.append(file_cursor, original_file_contents.end());

        if (write_file_to_disk(new_file_name, new_file_contents))
        {
            success_files.insert(new_file_name);
        }
        else
        {
            error_files.insert(new_file_name);
        }
    });
    return std::make_pair(success_files, error_files);
};

} // end namespace tb::functional
