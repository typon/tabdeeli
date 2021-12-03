#include <fn.hpp>
#include <fstream>
#include <algorithm>

#include <ftxui/dom/table.hpp>      // for Table, TableSelection

#include "serde.hpp"
#include "object_utils.hpp"
#include "components.hpp"
#include "utils.hpp"
#include "metaprogramming.hpp"

using namespace ftxui;

using ftxui_extras::StyledButton;

namespace tb
{

void
reset_state_before_search(AppState* app_state)
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
    file_picker->selected_file_index = 0;
    file_picker->file_to_matches.clear();
    file_picker->file_to_num_matches_found.clear();
    file_picker->file_names_as_displayed.clear();
    file_picker->file_names.clear();
    file_picker->file_managers.clear();
    file_picker->replacement_file_managers.clear();

    FileViewerState* file_viewer = &app_state->file_viewer_state;
    file_viewer->file_name = NO_FILE_LOADED;
    file_viewer->prev_lines.clear();
    file_viewer->new_lines.clear();
    file_viewer->preamble.clear();
    file_viewer->postamble.clear();

    BottomBarState* bottom_bar = &app_state->bottom_bar_state;
    bottom_bar->num_matches_found = 0;
    bottom_bar->num_matches_processed = 0;

    HistoryViewerState* history = &app_state->history_viewer_state;
    history->selected_diff = 0;
    history->diffs.clear();
    history->diffs_as_displayed.clear();

    FileCommitState* commit_state = &app_state->commit_state;
    commit_state->success_files.clear();
    commit_state->error_files.clear();
    commit_state->files_have_been_commmitted = false;
}

void
set_current_file_from_selected_diff(AppState* app_state)
{
    HistoryViewerState* history_state = &app_state->history_viewer_state;
    if (not history_has_diffs(history_state))
    {
        return;

    }

    S32 file_index = history_state->diffs.at(history_state->selected_diff).file_index;

    FilePickerState* file_picker = &app_state->file_picker_state;
    file_picker->selected_file_index = file_index;

    app_state->actions_queue.push_back(Action {ActionType::NoOp});
    app_state->screen->PostEvent(Event::Custom);
}

void
add_diff_to_history(AppState* state, HistoryViewerState* history, const TextDiff& diff)
{
    update_view_widths(state);
    history->diffs.push_back(diff);
    history->diffs_as_displayed.push_back(diff_display_item_from_diff(diff, history->current_width - 4));
}

ag_result::ag_match
pop_current_match(FilePickerState* file_picker)
{
    StringRef file_name = file_picker->file_names.at(file_picker->selected_file_index);
    auto result = file_picker->file_to_matches.at(file_name).front();
    file_picker->file_to_matches.at(file_name).pop_front();
    return result;
}

ByteSlice
get_current_match_byte_slice(AppState* app_state, FileViewerMode mode)
{
    if (mode == FileViewerMode::FILE_MATCH_VIEWER)
    {
        FilePickerState* file_picker = &app_state->file_picker_state;
        StringRef file_name = file_picker->file_names.at(file_picker->selected_file_index);
        return byte_slice_from_match(file_picker->file_to_matches.at(file_name).front());
    }
    else
    {
        HistoryViewerState* history_state = &app_state->history_viewer_state;
        TextDiff diff = history_state->diffs.at(history_state->selected_diff);
        return diff.byte_slice;
    }
}

void
delete_diff_from_history(AppState* app_state)
{
    HistoryViewerState* history = &app_state->history_viewer_state;
    if (not is_current_diff_valid(history))
    {
        return;
    }

    const TextDiff& selected_diff = history->diffs.at(history->selected_diff);
    FilePickerState* file_picker = &app_state->file_picker_state;
    StringRef file_name = selected_diff.file_name;

    ag_result::ag_match match = match_from_text_diff(selected_diff);

    file_picker->file_to_matches.at(file_name).push_back(match);

    history->diffs.erase(history->diffs.begin() + history->selected_diff);
    history->diffs_as_displayed.erase(history->diffs_as_displayed.begin() + history->selected_diff);

    // Update the global match process gauge
    app_state->bottom_bar_state.num_matches_processed--;

}

void
add_current_change_to_diff_history(AppState* app_state, B32 is_accepted)
{
    FilePickerState* file_picker = &app_state->file_picker_state;
    if (not files_have_been_loaded(file_picker))
    {
        return;
    }

    FileViewerState* file_viewer = &app_state->file_viewer_state;
    S32 selected_file_index = file_picker->selected_file_index;
    StringRef file_name = file_picker->file_names.at(selected_file_index);

    if (file_picker->file_names.size() <= selected_file_index)
    {
        return;
    }
    else if (all_matches_processed_for_current_file(file_picker))
    {
        return;
    }

    ag_result::ag_match current_match = pop_current_match(file_picker);
    ByteSlice current_match_byte_slice = byte_slice_from_match(current_match);
    std::optional<FileManager> file_manager_opt = functional::get_from_map_by_value(file_picker->file_managers, selected_file_index);
    FileLine first_line_of_match = get_lines_spanning_byte_slice(file_manager_opt.value(), current_match_byte_slice).at(0);

    auto diff = TextDiff {
        .accepted = is_accepted,
        .byte_slice = {.start = current_match.byte_start, .end = current_match.byte_end},
        .file_name = file_name,
        .file_index = selected_file_index,
        .replacement_text = app_state->bottom_bar_state.replacement_text,
        .start_line_no = first_line_of_match.lineno,
        .start_column = first_line_of_match.start_column,
    };

    HistoryViewerState* history = &app_state->history_viewer_state;
    add_diff_to_history(app_state, history, diff);

    // Update the global match process gauge
    app_state->bottom_bar_state.num_matches_processed++;
}

void
accept_all_changes_in_file(AppState* app_state)
{
    FilePickerState* file_picker = &app_state->file_picker_state;
    if (not files_have_been_loaded(file_picker))
    {
        return;
    }

    while (true)
    {
        add_current_change_to_diff_history(app_state, true);

        if (all_matches_processed_for_current_file(file_picker))
        {
            break;
        }
    }
}

void
reject_all_changes_in_file(AppState* app_state)
{
    FilePickerState* file_picker = &app_state->file_picker_state;
    if (not files_have_been_loaded(file_picker))
    {
        return;
    }

    while (true)
    {
        add_current_change_to_diff_history(app_state, false);

        if (all_matches_processed_for_current_file(file_picker))
        {
            break;
        }
    }
}

void
populate_file_viewer_state(AppState* app_state, FileViewerMode mode)
{
    FilePickerState* file_picker = &app_state->file_picker_state;
    FileViewerState* file_viewer = &app_state->file_viewer_state;
    HistoryViewerState* history_viewer = &app_state->history_viewer_state;
    file_viewer->mode = mode;
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
    else if (mode == FileViewerMode::HISTORY_DIFF_VIEWER and not history_has_diffs(history_viewer))
    {
        return;
    }

    StringRef file_name = file_picker->file_names.at(selected_file_index);

    if (all_matches_processed_for_file(file_picker, file_name)
        and mode == FileViewerMode::FILE_MATCH_VIEWER)
    {
        file_viewer->prev_lines.clear();
        file_viewer->new_lines.clear();
        return;
    }

    ByteSlice current_match_byte_slice = get_current_match_byte_slice(app_state, mode);

    std::optional<FileManager> file_manager_opt = functional::get_from_map_by_value(file_picker->file_managers, selected_file_index);
    if (not file_manager_opt.has_value())
    {
        file_manager_opt.emplace(read_file_into_file_manager(file_name));
        file_picker->file_managers.insert({selected_file_index, file_manager_opt.value()});
    }

    file_viewer->prev_lines = get_lines_spanning_byte_slice(file_manager_opt.value(), current_match_byte_slice);

    String replacement_text;
    if (mode == FileViewerMode::FILE_MATCH_VIEWER)
    {
        replacement_text = app_state->bottom_bar_state.replacement_text;
    }
    else
    {
        HistoryViewerState* history_state = &app_state->history_viewer_state;
        TextDiff diff = history_state->diffs.at(history_state->selected_diff);
        replacement_text = diff.replacement_text;
    }

    FileManager replacement_file_manager = replace_text_in_file(file_manager_opt.value(), current_match_byte_slice, replacement_text);

    auto replacement_slice = ByteSlice {.start = current_match_byte_slice.start, .end = current_match_byte_slice.start + replacement_text.length()};
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
        std::deque<ag_result::ag_match> matches;
        for (U64 j = 0; j < searcher->results[i]->nmatches; j++)
        {
            ag_result::ag_match match = *searcher->results[i]->matches[j];
            matches.push_back(match);
            num_matches_found++;
        }
        String file_name = String(searcher->results[i]->file);
        file_picker->file_to_matches[file_name] = matches;
    }

    app_state->bottom_bar_state.num_matches_found = num_matches_found;

    file_picker->file_names = functional::keys(file_picker->file_to_matches) % fn::to_vector();
    file_picker->file_names_as_displayed = functional::truncate_strings(file_picker->file_names, file_picker->current_width - 4);

    for (U64 i = 0; i < file_picker->file_names.size(); i++)
    {
        file_picker->file_to_num_matches_found.push_back(file_picker->file_to_matches.at(file_picker->file_names.at(i)).size());
    }

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
        auto logo_section = bold(text("tabdeeli"));

        return hbox({
            logo_section | center | flex,
        });
    });
}

BottomBarComponent
BottomBar(AppState* app_state, ScreenInteractive* screen, BottomBarState* state)
{

    auto search_button = StyledButton(bgcolor(Color::Blue),
        [] () {
            return hbox({underlined(color(Color::DarkBlue, text("S"))), text("earch")});
        },
        [app_state, state] () {
            log(app_state, "pre, presetting state");
            reset_state_before_search(app_state);
            log(app_state, "pre, executing search");
            if (state->search_text.empty())
            {
                return;
            }
            searcher::execute_search(&app_state->searcher, &app_state->logger, state->search_text, state->search_directory);
            log(app_state, "pre, populating picker state");
            populate_file_picker_state(app_state);
        }
    );

    auto commit_button = StyledButton(bgcolor(Color::Green),
        [] () {
            return hbox({underlined(color(Color::DarkGreen, text("C"))), text("ommit")});
        },
        [app_state] () {
            const auto [success_files, error_files] = functional::commit_diffs_to_disk(app_state);
            FileCommitState* commit_state = &app_state->commit_state;
            commit_state->success_files = success_files;
            commit_state->error_files  = error_files;

            std::vector<std::vector<String>> table = {
                {"File name", "Status"}
            };

            for (const auto& file: error_files)
            {
                table.push_back({file, "✗"});
            }
            for (const auto& file: success_files)
            {
                table.push_back({file, "✓"});
            }
            commit_state->files_table = table;
            commit_state->files_have_been_commmitted = true;
        }
    );

    auto cancel_button = StyledButton(bgcolor(Color::RedLight),
        [] () {
            return hbox({text("Cance"), underlined(color(Color::DarkRed, text("l")))});
        },
        [app_state] () {
            log(app_state, "pre, canceled");
        }
    );

    InputOption search_text_input_option;
    auto search_text_input = ftxui_extras::FlexibleInput(&state->search_text, "Enter search regex...", search_text_input_option);
    auto replacement_text_input = ftxui_extras::FlexibleInput(&state->replacement_text, "Enter replacement text...", search_text_input_option);
    auto search_directory_input = ftxui_extras::FlexibleInput(&state->search_directory, "Enter search directory...", search_text_input_option);

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

    auto self = Renderer(layout, [
            state,
            search_text_input,
            replacement_text_input,
            search_directory_input,
            search_button,
            commit_button,
            cancel_button] () {

        auto mode_label = bold(text("Mode: " + replacement_mode_serialize(state->replacement_mode)));
        F32 percent_matches_processed = 0;
        if (state->num_matches_found > 0)
        {
            percent_matches_processed = state->num_matches_processed / F32(state->num_matches_found);
        }

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
FilePicker(AppState* app_state, ScreenInteractive* screen, FilePickerState* state)
{
    state->menu_options.style_normal = bgcolor(Color::Blue);
    state->menu_options.style_selected = bgcolor(Color::Yellow);
    state->menu_options.style_focused = bgcolor(Color::Red);
    state->menu_options.style_selected_focused = bgcolor(Color::Red) | bold;

    auto file_picker_menu = Menu(&state->file_names_as_displayed, &state->selected_file_index, &state->menu_options);

    auto result = Renderer(file_picker_menu, [app_state, state, file_picker_menu] () {
        if (file_picker_menu->Focused())
        {
            populate_file_viewer_state(app_state, FileViewerMode::FILE_MATCH_VIEWER);
        }

        auto selection_count_display = files_have_been_loaded(state) ?
            text(fmt::format(" - [{}/{}]", state->selected_file_index + 1, state->file_to_matches.size())) :
            nothing(text(""));

        return
            window(
                hbox({
                    underlined(color(Color::GrayLight, text("F"))),
                    text("iles"),
                    selection_count_display,
                }),
                file_picker_menu->Render() | vscroll_indicator | frame
            );
    });


    return result;
}

Component
FileViewer(AppState* app_state, FileViewerState* state)
{
    return Renderer([
            app_state,
            state
        ] () {
        FilePickerState* file_picker_state = &app_state->file_picker_state;

        U32 num_matches_found = get_num_matches_found_for_curr_file(file_picker_state);
        U32 num_matches_processed = get_num_matches_processed_for_curr_file(file_picker_state);

        F32 percent_matches_processed = 0;
        if (num_matches_found > 0)
        {
            percent_matches_processed = num_matches_processed / F32(num_matches_found);
        }

        auto prev_lines_view = vbox(functional::text_views_from_file_lines(state->prev_lines, Color::Red, " -"));
        auto new_lines_view = vbox(functional::text_views_from_file_lines(state->new_lines, Color::SeaGreen1, " +"));

        bool draw_buttons = state->prev_lines.size() > 0 and not app_state->commit_state.files_have_been_commmitted;

        Element match_choice_menu;
        if (not draw_buttons)
        {
            match_choice_menu =
                hbox({
                    filler(),
                });
        }
        else if (state->mode == FileViewerMode::FILE_MATCH_VIEWER)
        {
            match_choice_menu =
                hbox({
                    filler(),
                    hbox({
                        hbox({bold(color(Color::LightGreen, text("y"))), text("es")}),
                        separator(),
                        hbox({bold(color(Color::RedLight, text("n"))), text("o")}),
                        separator(),
                        hbox({text("ye"), bold(color(Color::LightGreen, text("s"))), text(" - all in file")}),
                        separator(),
                        hbox({text("n"), bold(color(Color::RedLight, text("o"))), text(" - all in file")}),
                    }) | border,
                    filler(),
                    window(text(fmt::format("File matches processed [{}/{}]", num_matches_processed, num_matches_found)), gauge(percent_matches_processed)) | size(WIDTH, GREATER_THAN, 15),
                });

        }
        else if (state->mode == FileViewerMode::HISTORY_DIFF_VIEWER and history_has_diffs(&app_state->history_viewer_state))
        {
            match_choice_menu =
                hbox({
                    filler(),
                    hbox({bold(color(Color::RedLight, text("D"))), text("elete from history")}) | border,
                    filler(),
                    window(text(fmt::format("File matches processed [{}/{}]", num_matches_processed, num_matches_found)), gauge(percent_matches_processed)) | size(WIDTH, GREATER_THAN, 15),
                });

        }
        else if (state->mode == FileViewerMode::HISTORY_DIFF_VIEWER and not history_has_diffs(&app_state->history_viewer_state))
        {
            match_choice_menu =
                hbox({
                    filler(),
                    window(text(fmt::format("File matches processed [{}/{}]", num_matches_processed, num_matches_found)), gauge(percent_matches_processed)) | size(WIDTH, GREATER_THAN, 15),
                });
        }
        else
        {
            match_choice_menu =
                hbox({
                    filler(),
                    hbox({text("?")}),
                    filler(),
                    window(text(fmt::format("File matches processed [{}/{}]", num_matches_processed, num_matches_found)), gauge(percent_matches_processed)) | size(WIDTH, GREATER_THAN, 15),
                });

        }

        Element curr_diff_view;
        if (app_state->commit_state.files_have_been_commmitted)
        {

            auto table = Table(app_state->commit_state.files_table);

            // Add border around everything
            table.SelectAll().Border(LIGHT);

            // Make header row bold with a double border.
            table.SelectRow(0).Decorate(bold);
            table.SelectRow(0).SeparatorVertical(LIGHT);
            table.SelectRow(0).Border(LIGHT);

            // Align right the "Status" column.
            table.SelectColumn(1).DecorateCells(center);

            curr_diff_view =
                window(
                    text("Files committed to disk"),
                    hbox({
                        filler(),
                        table.Render(),
                        filler(),
                    })
                ) | center;

        }
        else if (app_state->searcher.state == SearcherState::NO_SEARCH_EXECUTED)
        {
            curr_diff_view =
                hbox({
                    filler(),
                    text("No search executed...Press") | vcenter,
                    color(Color::Blue, text("Search")) | border,
                    text("button to start") | vcenter,
                    filler(),
                });
        }
        else if (app_state->searcher.state == SearcherState::NO_RESULTS_FOUND)
        {
            curr_diff_view =
                hbox({
                    filler(),
                    color(Color::Red, text("No results found...")) | vcenter,
                    filler(),
                });
        }
        else if (all_matches_processed_for_current_file(file_picker_state)
                 and state->mode == FileViewerMode::FILE_MATCH_VIEWER)
        {
            curr_diff_view =
                hbox({
                    filler(),
                    text(fmt::format("{}/{} matches processed for this file",
                            get_num_matches_processed_for_curr_file(file_picker_state),
                            get_num_matches_processed_for_curr_file(file_picker_state)
                    )),
                    filler(),
                });

        } else {
            curr_diff_view = vbox({
                prev_lines_view | yflex_grow,
                separatorHeavy(),
                new_lines_view | yflex_grow,
                }) | flex | vcenter;
        }

        return window(text(state->file_name),
                        vbox({
                            filler(),
                            curr_diff_view,
                            filler(),
                            match_choice_menu,
                        })
        );
    });
}

Component
DiffItem(const TextDiff& diff)
{
    return Renderer([&] () {
        return
        vbox({
            text(functional::truncate_string(diff.file_name, 10)),
            text(fmt::format("[{}, {}]", diff.byte_slice.start, diff.byte_slice.end))
        });
    });
}

Component
HistoryViewer(AppState* app_state, ScreenInteractive* screen, HistoryViewerState* state)
{
    state->menu_options.style_normal = bgcolor(Color::Blue);
    state->menu_options.style_selected = bgcolor(Color::Yellow);
    state->menu_options.style_focused = bgcolor(Color::Red);
    state->menu_options.style_selected_focused = bgcolor(Color::Red) | bold;

    bool is_focusable = false;
    auto history_list = ftxui_extras::FlexibleMenu(&state->diffs_as_displayed, &state->selected_diff, is_focusable, &state->menu_options);

    return Renderer(history_list, [app_state, state, history_list] {
        if (history_list->Focused())
        {
            set_current_file_from_selected_diff(app_state);
            populate_file_viewer_state(app_state, FileViewerMode::HISTORY_DIFF_VIEWER);
        }

        auto selection_count_display = state->diffs.size() == 0 ?
            nothing(text("")) :
            text(fmt::format(" - [{}/{}]", state->selected_diff + 1, state->diffs.size()));

        return window(
            hbox({
                underlined(color(Color::GrayLight, text("H"))),
                text("istory"),
                selection_count_display,
            }),
            history_list->Render() | vscroll_indicator | frame
        );
    });
}

AppComponent
App(AppState* state)
{

    auto top_bar = TopBar(&state->top_bar_state);
    auto file_picker = FilePicker(state, state->screen, &state->file_picker_state);
    auto file_viewer = FileViewer(state, &state->file_viewer_state);
    auto history_viewer = HistoryViewer(state, state->screen, &state->history_viewer_state);
    auto bottom_bar = BottomBar(state, state->screen, &state->bottom_bar_state);

    auto layout = Container::Vertical({
        top_bar,
        file_picker,
        file_viewer,
        history_viewer,
        bottom_bar.self,
    });

    auto self = Renderer(layout, [state, top_bar, file_picker, file_viewer, history_viewer, bottom_bar] () {

        update_view_widths(state);

        return vbox({
            top_bar->Render(),
            separator(),
            hbox({
                file_picker->Render() | size(WIDTH, GREATER_THAN, state->file_picker_state.current_width),
                file_viewer->Render() | size(WIDTH, GREATER_THAN, state->file_viewer_state.current_width) | flex,
                history_viewer->Render() | size(WIDTH, GREATER_THAN, state->history_viewer_state.current_width),
            }) | flex,
            separator(),
            bottom_bar.self->Render(),
        }) | border;
    });
    self = CatchEvent(self, [state, file_picker, history_viewer, bottom_bar] (Event event) {
        if (event.is_character()) {
            String character = event.character();
            if (file_picker->Focused())
            {
                if (character == "y")
                {
                    add_current_change_to_diff_history(state, true);
                }
                else if (character == "n")
                {
                    add_current_change_to_diff_history(state, false);
                }
                else if (character == "s")
                {
                    accept_all_changes_in_file(state);
                }
                else if (character == "o")
                {
                    reject_all_changes_in_file(state);
                }
            }
            else if (history_viewer->Focused())
            {
                if (character == "d")
                {
                    delete_diff_from_history(state);
                }
            }
        }
        else if (event == Event::Custom)
        {
            log(state, "custom event, focusing file picker");
            Action action = state->actions_queue.front();
            state->actions_queue.pop_front();
            switch (action.type)
            {
                case ActionType::FocusFilePicker: file_picker->TakeFocus();
                case ActionType::NoOp: {};
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
                else if (U32(event.input().at(0)) == 27 and U32(event.input().at(1)) == 'h')
                {
                    history_viewer->TakeFocus();
                }
            }
        }
        return false;
    });

    return AppComponent {
        .self = self,
        .bottom_bar = bottom_bar,
    };
}

}
