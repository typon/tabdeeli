#include "object_utils.hpp"
#include "utils.hpp"

namespace tb
{

ByteSlice
byte_slice_from_match(ag_result::ag_match match)
{
    return ByteSlice {.start = match.byte_start, .end = match.byte_end};
}

TextDiff
make_text_diff(ag_result::ag_match match, StringRef file_name, StringRef replacement_text)
{
    return TextDiff {
        .byte_slice = {.start = match.byte_start, .end = match.byte_end},
        .file_name = file_name,
        .replacement_text = replacement_text,
    };
}

std::tuple<String, String>
diff_display_item_from_diff(const TextDiff& diff)
{

    String short_file_name = functional::truncate_string(diff.file_name, 25);

    return {
        short_file_name,
        fmt::format("@ [{}, {}]", diff.byte_slice.start, diff.byte_slice.end)
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

} // end namespace tb
