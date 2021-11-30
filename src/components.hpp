#pragma once

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Menu, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, separator, bold, hcenter, vbox, hbox, gauge, Element, operator|, border

#include "state.hpp"

namespace tb
{

ftxui::Component TopBar(TopBarState* state);
BottomBarComponent BottomBar(AppState* app_state, ftxui::ScreenInteractive* screen, BottomBarState* state);
ftxui::Component FilePicker(ftxui::ScreenInteractive* screen, FilePickerState* state);
AppComponent App(AppState* state);
}

namespace ftxui_extras
{
ftxui::Component StyledButton(
    ftxui::Decorator default_style,
    std::function<ftxui::Element()> label_text_element_callback_,
    std::function<void()> on_click,
    ftxui::Ref<ftxui::ButtonOption> = {});

ftxui::Component FlexibleMenu(tb::StringPairsConstRef entries, int* selected, ftxui::Ref<ftxui::MenuOption> option);
ftxui::Elements flexible_paragraph(std::string longstring, tb::U32 max_line_width);
} // end namespace ftxui_extras
