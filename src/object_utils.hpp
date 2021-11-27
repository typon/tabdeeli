#pragma once

#include "sl_types.hpp"
#include "state.hpp"

namespace tb
{

ByteSlice
byte_slice_from_match(ag_result::ag_match match);

TextDiff make_text_diff(ag_result::ag_match match, StringRef file_name, StringRef replacement_text);

std::tuple<String, String> diff_display_item_from_diff(const TextDiff& diff);

U32 get_num_matches_for_curr_file(FilePickerState* file_picker);

U32 get_curr_match_for_curr_file(FilePickerState* file_picker);

} // end namespace tb
