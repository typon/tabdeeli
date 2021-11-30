#include <vector>
#include <map>
#include <deque>
#include <fmt/os.h>
#include <ftxui/component/component_options.hpp>  // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive

#include "sl_types.hpp"
#include "searcher.hpp"

#pragma once

namespace tb
{

const static String NO_FILE_LOADED = "No file loaded";

enum class ActionType
{
    FocusFilePicker,
    NoOp,
};

enum class FileViewerMode
{
    FILE_MATCH_VIEWER,
    HISTORY_DIFF_VIEWER,
};

enum class ReplacementMode
{
    REGEX,
    PYTHON,
};

struct ByteSlice
{
    U64 start;
    U64 end;
};

struct FileLine
{
    std::string content;
    U32 lineno;
    U64 start_column;
};

struct FileManager
{
    StringRef file_name;
    String contents;
    std::vector<U32> line_start_byte_indices;
};

struct Action {
    ActionType type;
    union {
        struct { int foo; };
    };
};

struct TextDiff
{
    B32 accepted;
    ByteSlice byte_slice;
    String file_name;
    S32 file_index;
    String replacement_text;
    U64 start_line_no;
    U64 start_column;
};

struct TopBarState
{
    U32 changes_processed;
    U32 total_changes;
};

struct BottomBarState
{
    String search_text;
    String replacement_text;
    String search_directory;
    String search_button_label;
    String commit_button_label;
    String cancel_button_label;
    ReplacementMode replacement_mode;
    U32 num_matches_processed;
    U32 num_matches_found;
};

struct FilePickerState
{
    S32 selected_file_index;
    std::map<String, std::deque<ag_result::ag_match>> file_to_matches;
    std::vector<U64> file_to_num_matches_found;
    std::vector<String> file_names_as_displayed;
    std::vector<StringRef> file_names;
    std::map<U32, FileManager> file_managers;
    std::map<U32, FileManager> replacement_file_managers;
    ftxui::MenuOption menu_options;

    U64 min_width;
    U64 max_width;
    F32 width_ratio;
    U32 current_width;
};

struct FileViewerState
{
    FileViewerMode mode;
    StringRef file_name;
    String preamble;
    std::vector<FileLine> prev_lines;
    std::vector<FileLine> new_lines;
    String postamble;

    F32 width_ratio;
    U32 current_width;
};

struct HistoryViewerState
{
    S32 selected_diff;
    std::vector<TextDiff> diffs;
    std::vector<std::tuple<String, String>> diffs_as_displayed;
    ftxui::MenuOption menu_options;

    U64 min_width;
    U64 max_width;
    F32 width_ratio;
    U32 current_width;
};

using Logger = fmt::v8::ostream;

struct AppState
{
    Logger logger;
    Searcher searcher;
    std::deque<Action> actions_queue;
    ftxui::ScreenInteractive* screen;
    TopBarState top_bar_state;
    FilePickerState file_picker_state;
    FileViewerState file_viewer_state;
    HistoryViewerState history_viewer_state;
    BottomBarState bottom_bar_state;
};


struct BottomBarComponent
{
    ftxui::Component self;
    ftxui::Component search_button;
    ftxui::Component commit_button;
    ftxui::Component cancel_button;
    ftxui::Component search_text_input;
};

struct AppComponent
{
    ftxui::Component self;
    BottomBarComponent bottom_bar;
};

}
