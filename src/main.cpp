#include <functional>  // for function
#include <memory>      // for allocator, __shared_ptr_access
#include <string>      // for string, basic_string, operator+, to_string
#include <vector>      // for vector

#include <fmt/core.h>
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Menu, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, separator, bold, hcenter, vbox, hbox, gauge, Element, operator|, border

#include "sl_types.hpp"

using namespace ftxui;

struct TopBarState
{
    U32 changes_processed = 0;
    U32 total_changes = 0;
};

struct AppState
{
    TopBarState top_bar_state;
};

Component TopBar(TopBarState* state)
{
    return Renderer([state] () {
        auto logo_section = hcenter(bold(text("tabdeeli")));

        if (state->total_changes == 0)
        {
            auto progress_bar = hbox({filler()});
            return hbox({
                logo_section,
            });
        }

        double progress = double(state->changes_processed) / double(state->total_changes);
        auto progress_bar = hbox({
            text("Changes processed: "),
            gauge(progress),
            text(fmt::format("{}/{}", state->changes_processed, state->total_changes)),
        });
        return hbox({
            logo_section,
            separator(),
            progress_bar | flex,
        });
    });
}

Component Rew()
{
    return Renderer([] () {
        auto logo_section = hcenter(bold(text("tabdeeli")));

        auto progress_bar = hbox({
            text("Changes processed: "),
            gauge(0.5),
            text(fmt::format("{}/{}", 1, 2))
        });

        return hbox({
            logo_section,
            separator(),
            progress_bar | flex,
        });
    });
}

Component App(AppState* state)
{

    auto top_bar = TopBar(&state->top_bar_state);
    // auto top_bar = Rew();

    // return Renderer(layout, [layout] () {
    return Renderer([top_bar] () {
        return vbox({
            top_bar->Render(),
        // }) | size(WIDTH, GREATER_THAN, 40) | border;
        }) | border;
    });
}

int main(int argc, const char* argv[]) {
    auto screen = ScreenInteractive::Fullscreen();

    AppState* app_state = new AppState{};

    auto app = App(app_state);

    screen.Loop(app);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
