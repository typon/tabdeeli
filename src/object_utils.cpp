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
        fmt::format("{} Line: {}, Col: {}",
            diff.accepted ? "✓" : "✗",
            diff.start_line_no + 1, // Line numbers are displayed 1-indexed
            diff.start_column + 1 // Line columns are displayed 1-indexed
        ),
    };
}

StringRef
curr_file_name(FilePickerState* file_picker)
{
    return file_picker->file_names.at(file_picker->selected_file_index);
}

U32
get_num_matches_found_for_curr_file(FilePickerState* file_picker)
{
    if (file_picker->file_names.size() == 0)
    {
        return 0;
    }
    return file_picker->file_to_num_matches_found.at(file_picker->selected_file_index);
}

U32
get_num_matches_processed_for_curr_file(FilePickerState* file_picker)
{
    if (file_picker->file_names.size() == 0)
    {
        return 0;
    }
    auto remaining = file_picker->file_to_matches.at(curr_file_name(file_picker)).size();
    auto found = get_num_matches_found_for_curr_file(file_picker);
    assert(found >= remaining);
    return found - remaining;
}

void
update_view_widths(AppState* state)
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

B32
all_matches_processed_for_file(FilePickerState* file_picker, StringRef file_name)
{
    return file_picker->file_to_matches.at(file_name).size() == 0;
}

B32
all_matches_processed_for_current_file(FilePickerState* file_picker)
{
    if (not files_have_been_loaded(file_picker))
    {
        return true;
    }
    return file_picker->file_to_matches.at(curr_file_name(file_picker)).size() == 0;
}

B32
files_have_been_loaded(FilePickerState* file_picker)
{
    if (file_picker->file_to_matches.size() == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

B32
history_has_diffs(HistoryViewerState* history)
{
    return history->diffs.size() > 0;
}

ag_result::ag_match
match_from_text_diff(const TextDiff& diff)
{
    return ag_result::ag_match {
        .byte_start = diff.byte_slice.start,
        .byte_end = diff.byte_slice.end,
        .match = nullptr,
    };
}

B32
is_current_diff_valid(HistoryViewerState* history)
{
    return history_has_diffs(history) and (history->selected_diff < history->diffs.size());
}


} // end namespace tb
