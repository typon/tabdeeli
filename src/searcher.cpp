#include <cstring>

#include "utils.hpp"
#include "searcher.hpp"

namespace tb
{

Searcher::Searcher()
{
    /* 4 workers and enable binary files search. */
    std::memset(&this->config, 0, sizeof(struct ag_config));
	this->config.search_binary_files = 0;
	this->config.num_workers = 4;
    ag_init_config(&this->config);
    this->state = SearcherState::NO_SEARCH_EXECUTED;
    this->num_results = 0;
}

Searcher::~Searcher()
{
    if (this->results != nullptr)
    {
        ag_free_all_results(this->results, this->num_results);
        ag_finish();
    }
    this->num_results = 0;
    this->results = nullptr;
}

namespace searcher
{

B32 is_regex_invalid(StringRef search_text)
{
	B32 result = ag_is_regex_valid(const_cast<char*>(search_text.get().data())) == -2;
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
