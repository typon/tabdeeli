#include <functional>         // for function
#include <memory>             // for allocator, __shared_ptr_access
#include <string>             // for string, basic_string, operator+, to_string
#include <vector>             // for vector
#include <unordered_map>      // for unordered_map
#include <sstream>            // for std::stringstream, std::stringbuf


#include <fmt/core.h>                          // for fmt::format
#include <fmt/os.h>

#include <clipp.h>

#include "components.hpp"
#include "searcher.hpp"

using namespace ftxui;
using namespace tb;

extern "C" {
#include <libag.h>
}

int main(int argc, char* argv[]) {

    String search_text;
    String replacement_text;
    String search_directory = "./";

    auto cli = clipp::with_prefixes_short_long("-", "--",
        clipp::option("s", "search-regex").doc("search regex string") & clipp::value("search_regex", search_text),
        clipp::option("r", "replacement-string").doc("replacement text string") & clipp::value("replacement_string", replacement_text),
        clipp::option("d", "search-directory").doc("top level directory to launch search") & clipp::value("search_directory", search_directory)
    );

    auto parse_result = clipp::parse(argc, argv, cli);
    if (not parse_result)
    {
        std::stringstream ss;
        ss << make_man_page(cli, argv[0]);
        fmt::print("{}", ss.str());
        return -1;
    }

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
            .selected_file_index = 0,
            .file_to_matches = {},
            .file_names_as_displayed = {},
            .file_names = {},
            .min_width = 40,
            .max_width = 60,
            .width_ratio = F32(1)/F32(5),
            .current_width = 0,
        },
        .file_viewer_state = FileViewerState {
            .mode = FileViewerMode::FILE_MATCH_VIEWER,
            .file_name = NO_FILE_LOADED,
            .preamble = "",
            .prev_lines = {},
            .new_lines = {},
            .postamble = "",
            .width_ratio = F32(3)/F32(5),
            .current_width = 0,
        },
        .history_viewer_state = HistoryViewerState {
            .selected_diff = 0,
            .diffs = {},
            .diffs_as_displayed = {},
            .min_width = 30,
            .max_width = 100,
            .width_ratio = F32(1)/F32(5),
            .current_width = 0,
        },
        .bottom_bar_state = BottomBarState {
            .search_text = search_text,
            .replacement_text = replacement_text,
            .search_directory = search_directory,
            .search_button_label = "Search",
            .commit_button_label = "Commit",
            .cancel_button_label = "Cancel",
            .replacement_mode = ReplacementMode::REGEX,
        },
    };

    auto app = App(app_state);

    screen.Loop(app.self);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
