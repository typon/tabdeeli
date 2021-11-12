#include <fn.hpp>
#include <fstream>

#include "serde.hpp"
#include "object_utils.hpp"
#include "components.hpp"
#include "ftxui/component/component_base.hpp"
#include "utils.hpp"

using namespace ftxui;

using ftxui_extras::StyledButton;

namespace tb
{

void
populate_file_viewer_state(AppState* app_state)
{
    FilePickerState* file_picker = &app_state->file_picker_state;
    FileViewerState* file_viewer = &app_state->file_viewer_state;
    bool have_files = app_state->file_picker_state.file_names.size() > 0;
    U32 selected_file_index = file_picker->selected_file_index;
    file_viewer->file_name =
        have_files ?
            file_picker->file_names.at(selected_file_index) :
            std::cref(NO_FILE_LOADED);

    if (not have_files)
    {
        return;
    }

    StringRef file_name = file_picker->file_names.at(selected_file_index);
    U32 match_index = file_picker->file_to_currently_selected_match.at(selected_file_index);
    ag_result::ag_match match = file_picker->file_to_matches.at(file_name).at(match_index);

    std::optional<FileManager> file_manager_opt = functional::get_from_map_by_value(file_picker->file_managers, selected_file_index);
    if (not file_manager_opt.has_value())
    {
        file_manager_opt.emplace(read_file_into_file_manager(file_name));
        file_picker->file_managers.insert({selected_file_index, file_manager_opt.value()});
    }
    const auto& [prev_line, new_line] = get_surrounding_lines_for_byte_slice(file_manager_opt.value(), byte_slice_from_match(match));


    log(app_state, "lew ");

}

void
populate_file_picker_state(AppState* app_state)
{
    FilePickerState* file_picker = &app_state->file_picker_state;
    Searcher* searcher = &app_state->searcher;

    for (U64 i = 0; i < searcher->num_results; i++)
    {
        std::vector<ag_result::ag_match> matches;
        for (U64 j = 0; j < searcher->results[i]->nmatches; j++)
        {
            ag_result::ag_match match = *searcher->results[i]->matches[j];
            matches.push_back(match);
        }
        file_picker->file_to_matches[String(searcher->results[i]->file)] = matches;
    }

    // std::vector<std::reference_wrapper<const String>> file_names;
    // for (const auto&[fname, _] : file_picker->file_to_matches)
    // {
    //     file_names.push_back(std::cref(fname));
    // }
    // file_picker->file_names = file_names;
    U32 num_files = file_picker->file_to_matches.size();
    file_picker->file_names = functional::keys(file_picker->file_to_matches) % fn::to_vector();
    file_picker->file_to_currently_selected_match.assign(num_files, 0);
    file_picker->file_names_as_displayed = functional::truncate_strings(file_picker->file_names, file_picker->min_width);

    app_state->actions_queue.push_back(Action {ActionType::FocusFilePicker});
    app_state->screen->PostEvent(Event::Custom);

    /* for (const auto& [filename, results]: file_to_results) */
    /* { */
    /*     .push_back(filename); */
    /* } */
}

Component
Window(String title, Component component) {
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

    auto search_button = StyledButton(&state->search_button_label, bgcolor(Color::Blue), [app_state] () {
        searcher::execute_search(&app_state->searcher, &app_state->logger);
        log(app_state, "pre, populating picker state");
        populate_file_picker_state(app_state);
    });

    auto commit_button = StyledButton(&state->commit_button_label, bgcolor(Color::Green), [app_state] () {
        log(app_state, "pre, committed");
    });

    auto cancel_button = StyledButton(&state->cancel_button_label, bgcolor(Color::RedLight), [app_state] () {
        log(app_state, "pre, canceled");
    });

    auto layout = Container::Horizontal({
        search_button,
        commit_button,
        cancel_button,
    });

    return Renderer(layout, [state, search_button, commit_button, cancel_button] () {
        auto mode_label = bold(text("Mode: " + replacement_mode_serialize(state->replacement_mode)));

        return hbox({
            vcenter(mode_label) | flex,
            separator(),
            search_button->Render(),
            commit_button->Render(),
            cancel_button->Render(),
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
    auto result = Window("Files", Menu(&state->file_names_as_displayed, &state->selected_file_index, &state->menu_options));
    return result;
}

Component
FileViewer(AppState* app_state, FileViewerState* state)
{
    return Renderer([app_state, state] () {
        populate_file_viewer_state(app_state);

        return window(text(state->file_name),
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
HistoryViewer(ScreenInteractive* screen, HistoryViewerState* state)
{
    state->menu_options.style_normal = bgcolor(Color::Blue);
    state->menu_options.style_selected = bgcolor(Color::Yellow);
    state->menu_options.style_focused = bgcolor(Color::Red);
    state->menu_options.style_selected_focused = bgcolor(Color::Red);
    state->menu_options.on_enter = screen->ExitLoopClosure();
    auto result = Window("History", Menu(&state->diffs_as_displayed, &state->selected_diff, &state->menu_options));
    return result;
}

Component
App(AppState* state)
{

    auto top_bar = TopBar(&state->top_bar_state);
    auto file_picker = FilePicker(state->screen, &state->file_picker_state);
    auto file_viewer = FileViewer(state, &state->file_viewer_state);
    auto history_viewer = HistoryViewer(state->screen, &state->history_viewer_state);
    auto bottom_bar = BottomBar(state, state->screen, &state->bottom_bar_state);

    auto layout = Container::Vertical({
        top_bar,
        file_picker,
        file_viewer,
        history_viewer,
        bottom_bar,
    });

    auto result = Renderer(layout, [state, top_bar, file_picker, file_viewer, history_viewer, bottom_bar] () {
        return vbox({
            top_bar->Render(),
            separator(),
            hbox({
                file_picker->Render() | size (WIDTH, LESS_THAN, state->file_picker_state.max_width) | size(WIDTH, GREATER_THAN, state->file_picker_state.min_width),
                file_viewer->Render() | flex,
                history_viewer->Render() | size (WIDTH, LESS_THAN, state->history_viewer_state.max_width) | size(WIDTH, GREATER_THAN, state->history_viewer_state.min_width),
            }) | flex,
            separator(),
            bottom_bar->Render(),
        }) | border;
    });
    result = CatchEvent(result, [state, file_picker] (Event event) {
        if (event.is_character()) {
            String character = event.character();
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
