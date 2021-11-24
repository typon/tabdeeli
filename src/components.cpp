#include <fn.hpp>
#include <fstream>

#include "serde.hpp"
#include "object_utils.hpp"
#include "components.hpp"
#include "ftxui/component/component_base.hpp"
#include "utils.hpp"
#include "metaprogramming.hpp"

using namespace ftxui;

using ftxui_extras::StyledButton;

namespace tb
{

void reset_state_before_search(AppState* app_state)
{

    Searcher* searcher = &app_state->searcher;
    if (searcher->results != nullptr)
    {
        log(app_state, "Cleaning up ag resources");
        ag_free_all_results(searcher->results, searcher->num_results);
        searcher->num_results = 0;
        ag_finish();
        *searcher = searcher::init_searcher();
    }

    FilePickerState* file_picker = &app_state->file_picker_state;
    file_picker->file_to_matches.clear();
    file_picker->file_to_currently_selected_match.clear();
    file_picker->file_names.clear();
    file_picker->file_managers.clear();
    file_picker->file_names_as_displayed.clear();

    FileViewerState* file_viewer = &app_state->file_viewer_state;
    file_viewer->prev_lines.clear();
    file_viewer->new_lines.clear();
    file_viewer->preamble.clear();
    file_viewer->postamble.clear();
    file_viewer->current_diff_index = 0;
}

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

    std::string x = file_manager_opt.value().contents;
    if (std::string(file_manager_opt.value().file_name) == "/home/typon/gitz/tabdeeli/src/object_utils.cpp")
    {
        x = x.substr(0, 90);
    }
    else {
    x = x.substr(0, 40);
    }
    assert(x.size() > 0);

    log(app_state, "populating file viewer state");
    file_viewer->prev_lines = get_lines_spanning_byte_slice(file_manager_opt.value(), byte_slice_from_match(match));

    FileManager replacement_file_manager = replace_text_in_file(file_manager_opt.value(), byte_slice_from_match(match), app_state->bottom_bar_state.replacement_text);

    auto replacement_slice = ByteSlice {.start = match.byte_start, .end = match.byte_start + app_state->bottom_bar_state.replacement_text.length()};
    log(app_state, "replaced file: ");
    log(app_state, replacement_file_manager.contents);
    log(app_state, meta::to_string(replacement_slice));
    file_viewer->new_lines = get_lines_spanning_byte_slice(replacement_file_manager, replacement_slice);
    file_picker->replacement_file_managers.insert({selected_file_index, replacement_file_manager});
}

void
populate_file_picker_state(AppState* app_state)
{
    FilePickerState* file_picker = &app_state->file_picker_state;
    Searcher* searcher = &app_state->searcher;

    U32 num_matches_found = 0;

    for (U64 i = 0; i < searcher->num_results; i++)
    {
        std::vector<ag_result::ag_match> matches;
        for (U64 j = 0; j < searcher->results[i]->nmatches; j++)
        {
            ag_result::ag_match match = *searcher->results[i]->matches[j];
            matches.push_back(match);
            num_matches_found++;
        }
        file_picker->file_to_matches[String(searcher->results[i]->file)] = matches;
    }

    app_state->bottom_bar_state.num_matches_found = num_matches_found;

    U32 num_files = file_picker->file_to_matches.size();
    file_picker->file_names = functional::keys(file_picker->file_to_matches) % fn::to_vector();
    file_picker->file_to_currently_selected_match.assign(num_files, 0);
    file_picker->file_names_as_displayed = functional::truncate_strings(file_picker->file_names, file_picker->min_width);

    app_state->actions_queue.push_back(Action {ActionType::FocusFilePicker});
    app_state->screen->PostEvent(Event::Custom);

}

Component
Window(Element title, Component component) {
  return Renderer(component, [component, title] {  //
    return window(title, component->Render()) | flex;
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

BottomBarComponent
BottomBar(AppState* app_state, ScreenInteractive* screen, BottomBarState* state)
{

    auto search_button = StyledButton(&state->search_button_label,  bgcolor(Color::Blue),
        [] () {
            return hbox({underlined(color(Color::DarkBlue, text("S"))), text("earch")});
        },
        [app_state, state] () {
            log(app_state, "pre, presetting state");
            reset_state_before_search(app_state);
            log(app_state, "pre, executing search");
            searcher::execute_search(&app_state->searcher, &app_state->logger, state->search_text, state->search_directory);
            log(app_state, "pre, populating picker state");
            populate_file_picker_state(app_state);
        }
    );

    auto commit_button = StyledButton(&state->commit_button_label, bgcolor(Color::Green),
        [] () {
            return hbox({underlined(color(Color::DarkGreen, text("C"))), text("ommit")});
        },
        [app_state] () {
            log(app_state, "pre, committed");
        }
    );

    auto cancel_button = StyledButton(&state->cancel_button_label, bgcolor(Color::RedLight),
        [] () {
            return hbox({text("Cance"), underlined(color(Color::DarkRed, text("l")))});
        },
        [app_state] () {
            log(app_state, "pre, canceled");
        }
    );

    InputOption search_text_input_option;
    auto search_text_input = Input(&state->search_text, "Enter search regex...", search_text_input_option);
    auto replacement_text_input = Input(&state->replacement_text, "Enter replacement text...", search_text_input_option);
    auto search_directory_input = Input(&state->search_directory, "Enter search directory...", search_text_input_option);

    auto layout = Container::Horizontal({
        Container::Vertical({
            search_text_input,
            replacement_text_input,
            search_directory_input,
        }),
        search_button,
        commit_button,
        cancel_button,
    });

    F32 percent_matches_processed = 0;
    if (state->num_matches_found > 0)
    {
        percent_matches_processed = state->num_matches_processed / float(state->num_matches_found);
    }

    auto self = Renderer(layout, [
            percent_matches_processed,
            state,
            search_text_input,
            replacement_text_input,
            search_directory_input,
            search_button,
            commit_button,
            cancel_button] () {

        auto mode_label = bold(text("Mode: " + replacement_mode_serialize(state->replacement_mode)));

        return hbox({
            vbox({
                hbox({text("Search "), underlined(color(Color::GrayLight, text("r"))), text("egex: "), search_text_input->Render()}),
                hbox({text("Replacement string: "), replacement_text_input->Render()}),
                hbox({text("Search directory: "), search_directory_input->Render()}),
            })| flex,
            separator(),
            window(text(fmt::format("Matches processed [{}/{}]", state->num_matches_processed, state->num_matches_found)), gauge(percent_matches_processed)) | size(WIDTH, GREATER_THAN, 15),
            separator(),
            search_button->Render(),
            commit_button->Render(),
            cancel_button->Render(),
        });
    });

    return BottomBarComponent {
        .self = self,
        .search_button = search_button,
        .commit_button = commit_button,
        .cancel_button = cancel_button,
        .search_text_input = search_text_input,
    };
}

Component
FilePicker(ScreenInteractive* screen, FilePickerState* state)
{
    state->menu_options.style_normal = bgcolor(Color::Blue);
    state->menu_options.style_selected = bgcolor(Color::Yellow);
    state->menu_options.style_focused = bgcolor(Color::Red);
    state->menu_options.style_selected_focused = bgcolor(Color::Red);
    state->menu_options.on_enter = screen->ExitLoopClosure();


    /* auto file_picker_menu = Window(hbox({underlined(color(Color::GrayLight, text("F"))), text("iles")}) , Menu(&state->file_names_as_displayed, &state->selected_file_index, &state->menu_options)); */
    auto file_picker_menu = Menu(&state->file_names_as_displayed, &state->selected_file_index, &state->menu_options);

    auto result = Renderer(file_picker_menu, [file_picker_menu] () {
        return
            window(
                hbox({underlined(color(Color::GrayLight, text("F"))), text("iles")}),
                file_picker_menu->Render() | vscroll_indicator | frame
            );
    });


    return result;
}

Component
FileViewer(AppState* app_state, FileViewerState* state)
{
    auto accept_button = StyledButton(&state->accept_button_label, bgcolor(Color::Green),
        [] () {
            return hbox({bold(color(Color::DarkGreen, text("A"))), text("ccept")});
        },
        [app_state] () {
            log(app_state, "pre, accepted");
        }
    );

    auto reject_button = StyledButton(&state->reject_button_label, bgcolor(Color::RedLight),
        [] () {
            return hbox({bold(color(Color::DarkRed, text("R"))), text("eject")});
        },
        [app_state] () {
            log(app_state, "pre, canceled");
        }
    );

    auto layout = Container::Horizontal({
        accept_button,
        reject_button,
    });

    return Renderer(layout, [app_state, state, accept_button, reject_button] () {
        populate_file_viewer_state(app_state);

        auto prev_lines_view = vbox(functional::text_views_from_file_lines(state->prev_lines, Color::Red));
        /* auto new_lines_view = vbox(functional::text_views_from_file_lines(state->new_lines)); */
        auto new_lines_view = vbox(functional::text_views_from_file_lines(state->new_lines, Color::SeaGreen1));

        bool draw_buttons = state->prev_lines.size() > 0;

        Element match_choice_menu;
        if (draw_buttons)
        {
            match_choice_menu =
                hbox({
                    filler(),
                    accept_button->Render(),
                    reject_button->Render(),
                    filler(),
                });
        }
        else
        {
            match_choice_menu =
                hbox({
                    filler(),
                });
        }

        return window(text(state->file_name),
                        vbox({
                            /* hflow(paragraph(state->preamble)), */
                            /* filler(), */
                            prev_lines_view | yflex_grow,
                            new_lines_view | yflex_grow,
                            /* filler(), */
                            /* hflow(paragraph(state->postamble)), */
                            match_choice_menu,
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
    auto result = Window(text("History"), Menu(&state->diffs_as_displayed, &state->selected_diff, &state->menu_options));
    return result;
}

AppComponent
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
        bottom_bar.self,
    });

    auto self = Renderer(layout, [state, top_bar, file_picker, file_viewer, history_viewer, bottom_bar] () {
        return vbox({
            top_bar->Render(),
            separator(),
            hbox({
                file_picker->Render() | size (WIDTH, LESS_THAN, state->file_picker_state.max_width) | size(WIDTH, GREATER_THAN, state->file_picker_state.min_width),
                file_viewer->Render() | flex,
                history_viewer->Render() | size (WIDTH, LESS_THAN, state->history_viewer_state.max_width) | size(WIDTH, GREATER_THAN, state->history_viewer_state.min_width),
            }) | flex,
            separator(),
            bottom_bar.self->Render(),
        }) | border;
    });
    self = CatchEvent(self, [state, file_picker, bottom_bar] (Event event) {
        if (event.is_character()) {
            String character = event.character();
            log(state, "char event caught: " + character + "\n");
            if (character == "f")
            {
                log(state, "focusing file picker");
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
        else
        {
            // ALT = 27 (ASCII code)
            // s = 115 (ASCII code)
            if (event.input().size() == 2)
            {
                if (U32(event.input().at(0)) == 27 and U32(event.input().at(1)) == 's')
                {
                    bottom_bar.search_button->TakeFocus();
                }
                else if (U32(event.input().at(0)) == 27 and U32(event.input().at(1)) == 'c')
                {
                    bottom_bar.commit_button->TakeFocus();
                }
                else if (U32(event.input().at(0)) == 27 and U32(event.input().at(1)) == 'l')
                {
                    bottom_bar.cancel_button->TakeFocus();
                }
                else if (U32(event.input().at(0)) == 27 and U32(event.input().at(1)) == 'r')
                {
                    bottom_bar.search_text_input->TakeFocus();
                }
                else if (U32(event.input().at(0)) == 27 and U32(event.input().at(1)) == 'f')
                {
                    file_picker->TakeFocus();
                }
            }
            /* std::vector<U32> bytes = functional::string_to_bytes(event.input()) % functional::bytes_to_uints; */
            /* log(state, fmt::format(" event caught: {}", tb::meta::to_string(bytes))); */
        }
        return false;
    });

    return AppComponent {
        .self = self,
        .bottom_bar = bottom_bar,
    };
}

}
