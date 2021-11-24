#include <sstream>

#include "ftxui/component/component_base.hpp"
#include "ftxui/dom/node.hpp"

#include "utils.hpp"

using tb::U32;

namespace ftxui_extras {

using namespace ftxui;

Elements
flexible_paragraph(std::string longstring)
{

    auto initial_paragraph = paragraph(longstring);
    if (initial_paragraph.size() > 5)
    {
        // we have 5 individual text elements aka words in this paragraph, its probably good enough
        return initial_paragraph;
    }
    // We want to split into 40 chars
    U32 every_n_chars = 40;

    Elements result;
    for (const std::string& substr: tb::split_string(longstring, every_n_chars))
    {
        result.push_back(text(substr));
    }
    return result;
}

} // end namespace ftxui_extras
