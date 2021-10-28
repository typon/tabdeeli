#include <functional>         // for function
#include <memory>             // for allocator, __shared_ptr_access
#include <string>             // for string, basic_string, operator+, to_string
#include <vector>             // for vector
#include <unordered_map>      // for unordered_map

#include <fmt/core.h>                          // for fmt::format
#include <fmt/os.h>

#include "components.hpp"
#include "searcher.hpp"

using namespace ftxui;
using namespace tb;

extern "C" {
#include <libag.h>
}

int main(int argc, const char* argv[]) {

    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    AppState* app_state = new AppState {
        .logger = fmt::output_file("log.log"),
        .searcher = searcher::init_searcher(),
        .actions_queue = {},
        .screen = &screen,
        .top_bar_state = TopBarState {
            .changes_processed = 0,
            .total_changes = 0,
        },
        .file_picker_state = FilePickerState {
            .selected_file = 0,
            .file_names = {},
            .file_names_as_displayed = {},
            .min_width = 40,
            .max_width = 60,
        },
        .file_viewer_state = FileViewerState {
            .file_name = &NO_FILE_LOADED,
            .preamble = "",
            .prev_line = "",
            .new_line = "",
            .postamble = "",
        },
        .bottom_bar_state = BottomBarState {
            .search_button_label = "Search",
        },
    };

    auto app = App(app_state);

    screen.Loop(app);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
