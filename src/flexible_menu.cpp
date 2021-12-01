#include <stddef.h>    // for size_t
#include <algorithm>   // for max, min
#include <functional>  // for function
#include <memory>      // for shared_ptr, allocator_traits<>::value_type
#include <string>      // for operator+, string
#include <utility>     // for move
#include <vector>      // for vector, __alloc_traits<>::value_type

#include "ftxui/component/captured_mouse.hpp"     // for CapturedMouse
#include "ftxui/component/component.hpp"          // for Make, Menu
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowUp, Event::Return, Event::Tab, Event::TabReverse
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Released
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, Element, reflect, text, vbox, Elements, focus, nothing, select
#include "ftxui/screen/box.hpp"  // for Box
#include "ftxui/util/ref.hpp"    // for Ref

#include "sl_types.hpp"

using tb::StringPairsConstRef;

namespace ftxui_extras {

using namespace ftxui;

/// @brief A list of items. The user can navigate through them.
/// @ingroup component
class MenuBase : public ComponentBase {
 public:
  MenuBase(StringPairsConstRef entries, int* selected, bool is_focusable, Ref<MenuOption> option)
      : entries_(entries), selected_(selected), is_focusable_(is_focusable), option_(option) {}

    Element Render() override {
        Elements elements;
        bool is_menu_focused = Focused();
        boxes_.resize(entries_.size());
        for (size_t i = 0; i < entries_.size(); ++i) {
            bool is_focused = (focused_entry() == int(i)) && is_menu_focused;
            bool is_selected = (*selected_ == int(i));

            auto style = is_selected ? (is_focused ? option_->style_selected_focused
                                                   : option_->style_selected)
                                     : (is_focused ? option_->style_focused
                                                   : option_->style_normal);
            auto focus_management = !is_selected      ? nothing
                                    : is_menu_focused ? focus
                                                      : ftxui::select;
            auto icon = is_selected ? "> " : "  ";
            auto item = vbox({
                text(icon + std::get<0>(entries_[i])) | flex,
                text(std::get<1>(entries_[i])) | flex
            });
            /* auto item = text(icon + std::get<0>(entries_[i])); */
            elements.push_back(item | style | focus_management |
                               reflect(boxes_[i]));
        }
        return vbox(std::move(elements)) | reflect(box_);
    }


  bool OnEvent(Event event) override {
    if (!CaptureMouse(event))
      return false;

    if (event.is_mouse())
      return OnMouseEvent(event);

    if (Focused()) {
      int old_selected = *selected_;
      if (event == Event::ArrowUp || event == Event::Character('k'))
        (*selected_)--;
      if (event == Event::ArrowDown || event == Event::Character('j'))
        (*selected_)++;
      if (event == Event::Tab && entries_.size())
        *selected_ = (*selected_ + 1) % entries_.size();
      if (event == Event::TabReverse && entries_.size())
        *selected_ = (*selected_ + entries_.size() - 1) % entries_.size();

      *selected_ = std::max(0, std::min(int(entries_.size()) - 1, *selected_));

      if (*selected_ != old_selected) {
        focused_entry() = *selected_;
        option_->on_change();
        return true;
      }
    }

    if (event == Event::Return) {
      option_->on_enter();
      return true;
    }

    return false;
  }

  bool OnMouseEvent(Event event) {
    if (event.mouse().button == Mouse::WheelDown ||
        event.mouse().button == Mouse::WheelUp) {
      return OnMouseWheel(event);
    }

    if (event.mouse().button != Mouse::None &&
        event.mouse().button != Mouse::Left) {
      return false;
    }
    if (!CaptureMouse(event))
      return false;
    for (int i = 0; i < int(boxes_.size()); ++i) {
      if (!boxes_[i].Contain(event.mouse().x, event.mouse().y))
        continue;

      TakeFocus();
      focused_entry() = i;
      if (event.mouse().button == Mouse::Left &&
          event.mouse().motion == Mouse::Released) {
        if (*selected_ != i) {
          *selected_ = i;
          option_->on_change();
        }
        return true;
      }
    }
    return false;
  }

  bool OnMouseWheel(Event event) {
    if (!box_.Contain(event.mouse().x, event.mouse().y))
      return false;
    int old_selected = *selected_;

    if (event.mouse().button == Mouse::WheelUp)
      (*selected_)--;
    if (event.mouse().button == Mouse::WheelDown)
      (*selected_)++;

    *selected_ = std::max(0, std::min(int(entries_.size()) - 1, *selected_));

    if (*selected_ != old_selected)
      option_->on_change();
    return true;
  }

  bool Focusable() const final { return entries_.size() > 0 and this->is_focusable_; }
  int& focused_entry() { return option_->focused_entry(); }

 protected:
  StringPairsConstRef entries_;
  int* selected_ = 0;
  Ref<MenuOption> option_;
  bool is_focusable_ = true;

  std::vector<Box> boxes_;
  Box box_;
};

/// @brief A list of text. The focused element is selected.
/// @param entries The list of entries in the menu.
/// @param selected The index of the currently selected element.
/// @param option Additional optional parameters.
/// @ingroup component
/// @see MenuBase
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::TerminalOutput();
/// std::vector<std::string> entries = {
///     "entry 1",
///     "entry 2",
///     "entry 3",
/// };
/// int selected = 0;
/// auto menu = Menu(&entries, &selected);
/// screen.Loop(menu);
/// ```
///
/// ### Output
///
/// ```bash
/// > entry 1
///   entry 2
///   entry 3
/// ```
Component FlexibleMenu(StringPairsConstRef entries,
               int* selected,
               bool is_focusable,
               Ref<MenuOption> option) {
  return Make<MenuBase>(entries, selected, is_focusable, std::move(option));
}

}  // end namespace ftxui_extras
