#pragma once

#include <fn.hpp>

#include "state.hpp"

namespace fn = rangeless::fn;
using fn::operators::operator%;

namespace tb
{
void log(AppState* state, std::string_view msg);
void log(Logger* logger, std::string_view msg);
char** vector_of_strings_to_double_char_array(const std::vector<std::string>& strings);
}

namespace tb::functional
{

static auto keys = [](const auto& map)
{
    return
        fn::from(map.begin(), map.end())
        % fn::transform([](const auto& pair) {
            return std::move(pair.first);
        });
};

static auto sorted_keys = [](const auto& map)
{
    return
        keys(map)
        % fn::lazy_sort()
        % fn::unique_adjacent()
        % fn::to_vector();
};

inline std::string truncate_string(const std::string& str, U64 max_len)
{
    U64 len = str.length();
    if (len > max_len)
    {
        U64 diff = len - max_len;
        return "..." + str.substr(diff + 4, len);
    }
    return str;
}

static auto truncate_strings = [](const auto& strings, U64 max_len)
{
    return
        fn::from(strings.begin(), strings.end())
        % fn::transform([&](const auto& string) {
            return truncate_string(string, max_len);
        })
        % fn::to_vector();
};

} // end namespace tb::functional
