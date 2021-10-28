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
ftxui::Component BottomBar(AppState* app_state, ftxui::ScreenInteractive* screen, BottomBarState* state);
ftxui::Component FilePicker(ftxui::ScreenInteractive* screen, FilePickerState* state);
ftxui::Component App(AppState* state);

}
