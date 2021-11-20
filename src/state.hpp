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
    FocusFilePicker
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
};

struct FileManager
{
    StringRef file_name;
    String contents;
    std::vector<U32> line_start_byte_indices;
    std::vector<ByteSlice> match_byte_slices;
};

struct Action {
    ActionType type;
    union {
        struct { int foo; };
    };
};

struct TextDiff
{
    U32 start_byte;
    U32 end_byte;
    String file_name;
    String replacement_text;
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
};

struct FilePickerState
{
    S32 selected_file_index;
    std::map<String, std::vector<ag_result::ag_match>> file_to_matches;
    std::vector<U32> file_to_currently_selected_match;
    std::vector<String> file_names_as_displayed;
    std::vector<StringRef> file_names;
    std::map<U32, FileManager> file_managers;
    ftxui::MenuOption menu_options;

    U64 min_width;
    U64 max_width;
};

struct FileViewerState
{
    StringRef file_name;
    U32 current_diff_index;
    String preamble;
    std::vector<FileLine> prev_lines;
    std::vector<FileLine> new_lines;
    String postamble;
};

struct HistoryViewerState
{
    S32 selected_diff;
    std::vector<TextDiff> diffs;
    std::vector<String> diffs_as_displayed;
    ftxui::MenuOption menu_options;

    U64 min_width;
    U64 max_width;
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
