#include <cstring>

#include "utils.hpp"
#include "searcher.hpp"

namespace tb
{

namespace searcher
{

Searcher init_searcher()
{
    Searcher result;
    /* 4 workers and enable binary files search. */
    std::memset(&result.config, 0, sizeof(struct ag_config));
	result.config.search_binary_files = 0;
	result.config.num_workers = 4;
    ag_init_config(&result.config);
    result.state = SearcherState::NO_SEARCH_EXECUTED;

    return result;
}

void execute_search(Searcher* searcher, Logger* logger, StringRef search_text, StringRef search_directory)
{
    std::vector<String> paths = {search_directory};
	searcher->results = ag_search_ts(const_cast<char*>(search_text.get().data()), paths.size(), vector_of_strings_to_double_char_array(paths), &searcher->num_results);
	if (not searcher->results)
    {
        searcher->state = SearcherState::NO_RESULTS_FOUND;
		 
    }
	else
    {
        searcher->state = SearcherState::RESULTS_FOUND;
    }
}

} // end namespace searcher
} // end namespace tb
