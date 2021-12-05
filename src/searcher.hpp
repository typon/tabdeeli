#pragma once

#include "sl_types.hpp"
#include <fmt/os.h>

extern "C"
{
#include <libag.h>
}


namespace tb
{

using Logger = fmt::v8::ostream;

enum class SearcherState
{
    INVALID_REGEX,
    NO_RESULTS_FOUND,
    RESULTS_FOUND,
    NO_SEARCH_EXECUTED,
};

struct Searcher
{
    U64 num_results;
    ag_result** results = nullptr;
    ag_config config;
    SearcherState state;
};


namespace searcher
{
    Searcher init_searcher();
    void execute_search(Searcher* searcher, Logger* logger, StringRef search_text, StringRef search_directory);
    void reset_state(Searcher* searcher);
    S32 is_regex_invalid(Searcher* searcher, StringRef search_text);
}

}
