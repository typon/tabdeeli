#pragma once

#include "sl_types.hpp"
#include "state.hpp"

namespace tb
{

ByteSlice byte_slice_from_match(ag_result::ag_match match);
std::tuple<String, String> diff_display_item_from_diff(const TextDiff& diff, U32 view_width);
StringRef curr_file_name(FilePickerState* file_picker);
U32 get_num_matches_found_for_curr_file(FilePickerState* file_picker);
U32 get_num_matches_processed_for_curr_file(FilePickerState* file_picker);
void update_view_widths(AppState* state);
B32 all_matches_processed_for_file(FilePickerState* file_picker, StringRef file_name);
B32 all_matches_processed_for_current_file(FilePickerState* file_picker);
B32 files_have_been_loaded(FilePickerState* file_picker);

} // end namespace tb
