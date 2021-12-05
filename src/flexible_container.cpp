#include <stddef.h>   // for size_t
#include <algorithm>  // for max, min
#include <memory>  // for make_shared, __shared_ptr_access, allocator, shared_ptr, allocator_traits<>::value_type
#include <utility>  // for move
#include <vector>   // for vector, __alloc_traits<>::value_type

#include "ftxui/component/component.hpp"  // for Horizontal, Vertical, Tab
#include "ftxui/component/component_base.hpp"  // for Components, Component, ComponentBase
#include "ftxui/component/event.hpp"  // for Event, Event::Tab, Event::TabReverse, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::WheelDown, Mouse::WheelUp
#include "ftxui/dom/elements.hpp"  // for text, Elements, operator|, reflect, Element, hbox, vbox
#include "ftxui/screen/box.hpp"  // for Box

#include "components.hpp"

using namespace ftxui;

namespace ftxui_extras {

class ContainerBase : public ComponentBase {
 public:
  ContainerBase(Components children, bool is_focusable, int* selector)
      : is_focusable_(is_focusable), selector_(selector ? selector : &selected_) {
    for (Component& child : children)
      Add(std::move(child));
  }

  // Component override.
  bool OnEvent(Event event) override {
    if (event.is_mouse())
      return OnMouseEvent(event);

    if (!Focused())
      return false;

    if (ActiveChild() && ActiveChild()->OnEvent(event))
      return true;

    return EventHandler(event);
  }

  Component ActiveChild() override {
    if (children_.size() == 0)
      return nullptr;

    return children_[*selector_ % children_.size()];
  }

  void SetActiveChild(ComponentBase* child) override {
    for (size_t i = 0; i < children_.size(); ++i) {
      if (children_[i].get() == child) {
        *selector_ = i;
        return;
      }
    }
  }

 protected:
  // Handlers
  virtual bool EventHandler(Event) { return false; }

  virtual bool OnMouseEvent(Event event) {
    return ComponentBase::OnEvent(event);
  }

  bool is_focusable_ = true;
  int selected_ = 0;
  int* selector_ = nullptr;

  void MoveSelector(int dir) {
    for (int i = *selector_ + dir; i >= 0 && i < (int)children_.size();
         i += dir) {
      if (children_[i]->Focusable()) {
        *selector_ = i;
        return;
      }
    }
  }
  void MoveSelectorWrap(int dir) {
    for (size_t offset = 1; offset < children_.size(); ++offset) {
      int i = (*selector_ + offset * dir + children_.size()) % children_.size();
      if (children_[i]->Focusable()) {
        *selector_ = i;
        return;
      }
    }
  }
};

class VerticalContainer : public ContainerBase {
 public:
  using ContainerBase::ContainerBase;

  Element Render() override {
    Elements elements;
    for (auto& it : children_)
      elements.push_back(it->Render());
    if (elements.size() == 0)
      return text("Empty container") | reflect(box_);
    return vbox(std::move(elements)) | reflect(box_);
  }

  bool EventHandler(Event event) override {
    int old_selected = *selector_;
    if (event == Event::ArrowUp || event == Event::Character('k'))
      MoveSelector(-1);
    if (event == Event::ArrowDown || event == Event::Character('j'))
      MoveSelector(+1);
    if (event == Event::Tab && children_.size())
      MoveSelectorWrap(+1);
    if (event == Event::TabReverse && children_.size())
      MoveSelectorWrap(-1);

    *selector_ = std::max(0, std::min(int(children_.size()) - 1, *selector_));
    return old_selected != *selector_;
  }

  bool OnMouseEvent(Event event) override {
    if (ContainerBase::OnMouseEvent(event))
      return true;

    if (event.mouse().button != Mouse::WheelUp &&
        event.mouse().button != Mouse::WheelDown) {
      return false;
    }

    if (!box_.Contain(event.mouse().x, event.mouse().y))
      return false;

    if (event.mouse().button == Mouse::WheelUp)
      MoveSelector(-1);
    if (event.mouse().button == Mouse::WheelDown)
      MoveSelector(+1);
    *selector_ = std::max(0, std::min(int(children_.size()) - 1, *selector_));

    return true;
  }

  bool Focusable() const final
  {
    return this->is_focusable_ ? ContainerBase::Focusable() : false;
  }

  Box box_;
};

class HorizontalContainer : public ContainerBase {
 public:
  using ContainerBase::ContainerBase;

  Element Render() override {
    Elements elements;
    for (auto& it : children_)
      elements.push_back(it->Render());
    if (elements.size() == 0)
      return text("Empty container");
    return hbox(std::move(elements));
  }

  bool EventHandler(Event event) override {
    int old_selected = *selector_;
    if (event == Event::ArrowLeft || event == Event::Character('h'))
      MoveSelector(-1);
    if (event == Event::ArrowRight || event == Event::Character('l'))
      MoveSelector(+1);
    if (event == Event::Tab && children_.size())
      MoveSelectorWrap(+1);
    if (event == Event::TabReverse && children_.size())
      MoveSelectorWrap(-1);

    *selector_ = std::max(0, std::min(int(children_.size()) - 1, *selector_));
    return old_selected != *selector_;
  }
};

class TabContainer : public ContainerBase {
 public:
  using ContainerBase::ContainerBase;

  Element Render() override {
    Component active_child = ActiveChild();
    if (active_child)
      return active_child->Render();
    return text("Empty container");
  }

  bool OnMouseEvent(Event event) override {
    return ActiveChild()->OnEvent(event);
  }
};

/// @brief A list of components, drawn one by one vertically and navigated
/// vertically using up/down arrow key or 'j'/'k' keys.
/// @param children the list of components.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// auto container = Container::Vertical({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// });
/// ```
Component FlexibleVertical(Components children, bool is_focusable) {
  return FlexibleVertical(std::move(children), is_focusable, nullptr);
}

/// @brief A list of components, drawn one by one vertically and navigated
/// vertically using up/down arrow key or 'j'/'k' keys.
/// This is useful for implementing a Menu for instance.
/// @param children the list of components.
/// @param selector A reference to the index of the selected children.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// auto container = Container::Vertical({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// });
/// ```
Component FlexibleVertical(Components children, bool is_focusable, int* selector) {
  return std::make_shared<VerticalContainer>(std::move(children), is_focusable, selector);
}

/// @brief A list of components, drawn one by one horizontally and navigated
/// horizontally using left/right arrow key or 'h'/'l' keys.
/// @param children the list of components.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// int selected_children = 2;
/// auto container = Container::Horizontal({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// }, &selected_children);
/// ```
Component FlexibleHorizontal(Components children, bool is_focusable) {
  return FlexibleHorizontal(std::move(children), is_focusable, nullptr);
}

/// @brief A list of components, drawn one by one horizontally and navigated
/// horizontally using left/right arrow key or 'h'/'l' keys.
/// @param children the list of components.
/// @param selector A reference to the index of the selected children.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// int selected_children = 2;
/// auto container = Container::Horizontal({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// }, selected_children);
/// ```
Component FlexibleHorizontal(Components children, bool is_focusable, int* selector) {
  return std::make_shared<HorizontalContainer>(std::move(children), is_focusable, selector);
}

}  // namespace ftxui_extras
