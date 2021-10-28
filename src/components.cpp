#include <fn.hpp>
#include <fstream>

#include "components.hpp"
#include "ftxui/component/component_base.hpp"
#include "utils.hpp"

using namespace ftxui;

namespace tb
{

void
populate_file_viewer_state(AppState* app_state)
{
    FilePickerState* file_picker = &app_state->file_picker_state;
    FileViewerState* file_viewer = &app_state->file_viewer_state;
    bool have_files = app_state->file_picker_state.file_names.size() > 0;
    file_viewer->file_name =
        have_files ?
            &file_picker->file_names.at(file_picker->selected_file) :
            &NO_FILE_LOADED;
    if (not have_files)
    {
        return;
    }
    /* std::ifstream fh(file_picker->file_names.at(file_picker->selected_file)); */
}

void
populate_file_picker_state(AppState* app_state)
{
    FilePickerState* file_picker = &app_state->file_picker_state;
    Searcher* searcher = &app_state->searcher;

    std::unordered_map<std::string, std::vector<ag_result::ag_match>> file_to_results;

    for (U64 i = 0; i < searcher->num_results; i++)
    {
        std::vector<ag_result::ag_match> matches;
        for (U64 j = 0; j < searcher->results[i]->nmatches; j++)
        {
            ag_result::ag_match match = *searcher->results[i]->matches[j];
            matches.push_back(match);
        }
        file_to_results[std::string(searcher->results[i]->file)] = matches;
    }

    file_picker->file_names = functional::sorted_keys(file_to_results);
    file_picker->file_names_as_displayed = functional::truncate_strings(file_picker->file_names, file_picker->min_width);
    for (const auto& f: file_picker->file_names)
    {
        log(app_state, f);
    }

    app_state->actions_queue.push_back(Action {ActionType::FocusFilePicker});
    app_state->screen->PostEvent(Event::Custom);

    /* for (const auto& [filename, results]: file_to_results) */
    /* { */
    /*     .push_back(filename); */
    /* } */
}

Component
Window(std::string title, Component component) {
  return Renderer(component, [component, title] {  //
    return window(text(title), component->Render()) | flex;
  });
}

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
BottomBar(AppState* app_state, ScreenInteractive* screen, BottomBarState* state)
{

    auto search_button = Button(&state->search_button_label, [app_state] () {
        searcher::execute_search(&app_state->searcher, &app_state->logger);
        log(app_state, "pre, populating picker state");
        populate_file_picker_state(app_state);
    });

    auto layout = Container::Horizontal({
        search_button,
    });

    return Renderer(layout, [state, search_button] () {
        auto mode_label = bold(text("Mode: "));

        return hbox({
            vcenter(mode_label) | flex,
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
    auto result = Window("Files", Menu(&state->file_names_as_displayed, &state->selected_file, &state->menu_options));
    return result;
}

Component
FileViewer(AppState* app_state, FileViewerState* state)
{
    return Renderer([app_state, state] () {
        populate_file_viewer_state(app_state);

        return window(text(*state->file_name),
                        vbox({
                            hflow(paragraph(state->preamble)),
                            text(state->prev_line),
                            text(state->new_line),
                            hflow(paragraph(state->postamble)),
                        })
        );
    });
}

Component
App(AppState* state)
{

    auto top_bar = TopBar(&state->top_bar_state);
    auto file_picker = FilePicker(state->screen, &state->file_picker_state);
    auto file_viewer = FileViewer(state, &state->file_viewer_state);
    auto bottom_bar = BottomBar(state, state->screen, &state->bottom_bar_state);

    auto layout = Container::Vertical({
        top_bar,
        file_picker,
        file_viewer,
        bottom_bar,
    });

    auto result = Renderer(layout, [state, top_bar, file_picker, file_viewer, bottom_bar] () {
        return vbox({
            top_bar->Render(),
            separator(),
            hbox({
                file_picker->Render() | size (WIDTH, LESS_THAN, state->file_picker_state.max_width) | size(WIDTH, GREATER_THAN, state->file_picker_state.min_width),
                separator(),
                file_viewer->Render() | flex
            }) | flex,
            separator(),
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
        else if (event == Event::Custom)
        {
            log(state, "custom event");
            Action action = state->actions_queue.front();
            state->actions_queue.pop_front();
            switch (action.type)
            {
                case ActionType::FocusFilePicker: file_picker->TakeFocus();
                default: {}
            }
        }
        return false;
    });
    return result;
}

}
