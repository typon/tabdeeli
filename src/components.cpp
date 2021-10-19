#include "components.hpp"
#include "utils.hpp"

using namespace ftxui;

namespace tb
{

Component
TopBar(TopBarState* state)
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

Component
BottomBar(ScreenInteractive* screen, BottomBarState* state)
{

  auto search_button = Button(&state->start_button_label, [] () {

  });

  auto layout = Container::Horizontal({
      search_button,
  });

    return Renderer(layout, [state, search_button] () {
        auto mode_label = bold(text("Mode: "));

        return hbox({
            mode_label | flex,
            separator(),
            search_button->Render(),
        });
    });
}

Component
FilePicker(ScreenInteractive* screen, FilePickerState* state)
{
    state->menu_options.style_normal = bgcolor(Color::Blue);
    state->menu_options.style_selected = bgcolor(Color::Yellow);
    state->menu_options.style_focused = bgcolor(Color::Red);
    state->menu_options.style_selected_focused = bgcolor(Color::Red);
    state->menu_options.on_enter = screen->ExitLoopClosure();
    auto result = Menu(&state->file_names, &state->selected_file, &state->menu_options);
    return result;
}

Component
App(AppState* state)
{

    auto top_bar = TopBar(&state->top_bar_state);
    auto file_picker = FilePicker(state->screen, &state->file_picker_state);
    auto bottom_bar = BottomBar(state->screen, &state->bottom_bar_state);

    auto layout = Container::Vertical({
        top_bar,
        file_picker,
        bottom_bar,
    });

    auto result = Renderer(layout, [top_bar, file_picker, bottom_bar] () {
        return vbox({
            top_bar->Render(),
            file_picker->Render(),
            bottom_bar->Render(),
        }) | border;
    });
    result = CatchEvent(result, [state, file_picker] (Event event) {
        if (event.is_character()) {
            std::string character = event.character();
            log(state, "event caught: " + character + "\n");
            if (character == "f")
            {
                log(state, "focusing file picker");
                file_picker->TakeFocus();
            }
        }
        return false;
    });
    return result;
}

}
