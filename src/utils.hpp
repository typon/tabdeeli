#pragma once

#include <optional>
#include <fn.hpp>

#include "state.hpp"

namespace fn = rangeless::fn;
using fn::operators::operator%;

namespace tb
{
void log(AppState* state, std::string_view msg);
void log(Logger* logger, std::string_view msg);
Char** vector_of_strings_to_double_char_array(const std::vector<String>& strings);
FileManager read_file_into_file_manager(const String& file_name);
std::tuple<String, String> get_surrounding_lines_for_byte_slice(const FileManager&, ByteSlice);
}

namespace tb::functional
{

static auto keys = [](const auto& map)
{
    return
        fn::refs(map)
        % fn::transform([](const auto& pair) {
            return std::cref(pair.get().first);
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

static auto get_from_map = [](const auto& map, const auto& key)
{
    const auto& item_iterator = map.find(key);
    if (item_iterator == map.end())
    {
        return std::nullopt;
    }
    else
    {
        return std::make_optional(std::cref(item_iterator->second));
    }
};

// Returns an optional over the value stored in the map
static auto get_from_map_by_value = [](const auto& map, const auto& key)
    -> std::optional<typename std::remove_reference<decltype(map)>::type::mapped_type>
{
    const auto& item_iterator = map.find(key);
    if (item_iterator == map.end())
    {
        return std::nullopt;
    }
    else
    {
        return std::make_optional(item_iterator->second);
    }
};

} // end namespace tb::functional
