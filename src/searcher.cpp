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

void reset_state(Searcher* searcher)
{
    ag_free_all_results(searcher->results, searcher->num_results);
    searcher->num_results = 0;
    searcher->results = nullptr;
    ag_finish();
}

S32 is_regex_invalid(Searcher* searcher, StringRef search_text)
{
	S32 result = ag_is_regex_valid(const_cast<char*>(search_text.get().data())) == -2;
    reset_state(searcher);
    return result;
}

void execute_search(Searcher* searcher, Logger* logger, StringRef search_text, StringRef search_directory)
{
    std::vector<String> paths = {search_directory};

	Char** paths_as_char_arrays = vector_of_strings_to_double_char_array(paths);

	searcher->results = ag_search(const_cast<char*>(search_text.get().data()), paths.size(), paths_as_char_arrays, &searcher->num_results);
	if (not searcher->results)
    {
        searcher->state = SearcherState::NO_RESULTS_FOUND;
    }
	else
    {
        searcher->state = SearcherState::RESULTS_FOUND;
    }

    free(paths_as_char_arrays);
}

} // end namespace searcher
} // end namespace tb
