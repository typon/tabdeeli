#include "object_utils.hpp"
#include "utils.hpp"

namespace tb
{

ByteSlice
byte_slice_from_match(ag_result::ag_match match)
{
    return ByteSlice {.start = match.byte_start, .end = match.byte_end};
}

std::tuple<String, String>
diff_display_item_from_diff(const TextDiff& diff, U32 view_width)
{

    String short_file_name = functional::truncate_string(diff.file_name, view_width);

    return {
        short_file_name,
        fmt::format("@ Line: {}, Col: {}",
            diff.start_line_no + 1, // Line numbers are displayed 1-indexed
            diff.start_column + 1 // Line columns are displayed 1-indexed
        ),
    };
}

U32 get_num_matches_for_curr_file(FilePickerState* file_picker)
{
    if (file_picker->file_names.size() == 0)
    {
        return 0;
    }
    return file_picker->file_to_matches.at(file_picker->file_names.at(file_picker->selected_file_index)).size();
}

U32 get_curr_match_for_curr_file(FilePickerState* file_picker)
{
    if (file_picker->file_names.size() == 0)
    {
        return 0;
    }
    return file_picker->file_to_currently_selected_match.at(file_picker->selected_file_index);
}

void update_view_widths(AppState* state)
{
    auto terminal_x_size = ftxui::Terminal::Size().dimx;
    auto widths = split_term_x_into_three_by_ratios(
        terminal_x_size,
        state->file_picker_state.width_ratio,
        state->file_viewer_state.width_ratio,
        state->history_viewer_state.width_ratio
    );
    state->file_picker_state.current_width = widths.at(0);
    state->file_viewer_state.current_width = widths.at(1);
    state->history_viewer_state.current_width = widths.at(2);
}

} // end namespace tb
