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

struct Searcher
{
    U64 num_results;
    ag_result** results = nullptr;
    ag_config config;
};


namespace searcher
{
    Searcher init_searcher();
    void execute_search(Searcher* searcher, Logger* logger, StringRef search_text, StringRef search_directory);
}

}
