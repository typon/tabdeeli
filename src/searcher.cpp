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

    return result;
}

void execute_search(Searcher* searcher, Logger* logger)
{
    std::vector<String> paths = {"/home/typon/gitz/tabdeeli/src"};
	searcher->results = ag_search("namespace", paths.size(), vector_of_strings_to_double_char_array(paths), &searcher->num_results);
	if (not searcher->results)
    {
		log(logger, "no results_found");
    }
	else
    {
		log(logger, fmt::format("{} results found", searcher->num_results));
        /* Show them on the screen, if any. */
        for (size_t i = 0; i < searcher->num_results; i++) {
            for (size_t j = 0; j < searcher->results[i]->nmatches; j++) {
                log(logger, fmt::format("file: {}, match: {}, byte_start: {}, byte_end: {}",
                                searcher->results[i]->file,
                                searcher->results[i]->matches[j]->match,
                                searcher->results[i]->matches[j]->byte_start,
                                searcher->results[i]->matches[j]->byte_end
                ));
            }
        }
    }
}

} // end namespace searcher
} // end namespace tb
