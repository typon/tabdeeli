#include <vector>
#include <fmt/os.h>
#include <ftxui/component/component_options.hpp>  // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive

#include "sl_types.hpp"

#pragma once

namespace tb
{

struct TopBarState
{
    U32 changes_processed;
    U32 total_changes;
};

struct BottomBarState
{
    std::string start_button_label;
};

struct FilePickerState
{
    S32 selected_file;
    std::vector<std::string> file_names;
    std::vector<std::string> file_contents;
    ftxui::MenuOption menu_options;
};

struct AppState
{
    fmt::v8::ostream logger;
    ftxui::ScreenInteractive* screen;
    TopBarState top_bar_state;
    FilePickerState file_picker_state;
    BottomBarState bottom_bar_state;
};

}
