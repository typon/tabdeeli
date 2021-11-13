#pragma once

#include <type_traits>
#include <rttr/type>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace tt::meta
{

using namespace rttr;
json to_json_variant(const variant& object);
json to_json(instance object);
std::string to_string(instance object);
bool generic_eq_operator(instance a, instance b);
bool generic_neq_operator(instance a, instance b);

template<int N, typename... Ts>
using NthTypeOf = typename std::tuple_element<N, std::tuple<Ts...>>::type;

template <typename... Result, std::size_t... Indices>
auto vec_to_tup_helper(std::vector<variant> v, std::index_sequence<Indices...>) {
    return std::make_tuple(
	    variant_cast<NthTypeOf<Indices, Result...>>(v[Indices])...
    );
}

template <typename ...Result>
std::tuple<Result...> vec_to_tup(std::vector<variant> values)
{
    return vec_to_tup_helper<Result...>(values, std::make_index_sequence<sizeof...(Result)>());
}

} // end namespace tt::meta

namespace tt
{

template <typename T>
struct Hasher
{
    std::size_t operator()(const T& obj) const
    {
        return std::hash<std::string>()(tt::meta::to_string(obj));
    }
};

} //end namespace tt
