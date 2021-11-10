#include <vector>
#include <deque>
#include <fmt/os.h>
#include <ftxui/component/component_options.hpp>  // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive

#include "sl_types.hpp"
#include "searcher.hpp"

#pragma once

namespace tb
{

const static std::string NO_FILE_LOADED = "No file loaded";

enum class ActionType
{
    FocusFilePicker
};

struct Action {
    ActionType type;
    union {
        struct { int foo; };
    };
};

struct TopBarState
{
    U32 changes_processed;
    U32 total_changes;
};

struct BottomBarState
{
    std::string search_button_label;
    std::string commit_button_label;
    std::string cancel_button_label;
};

struct FilePickerState
{
    S32 selected_file;
    std::vector<std::string> file_names;
    ftxui::MenuOption menu_options;
    std::vector<std::string> file_names_as_displayed;

    U64 min_width;
    U64 max_width;
};

struct FileViewerState
{
    const std::string* file_name;
    std::string preamble;
    std::string prev_line;
    std::string new_line;
    std::string postamble;
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
    BottomBarState bottom_bar_state;
};

}
