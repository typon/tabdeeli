#include "state.hpp"

namespace tb
{

void log(AppState* state, std::string_view msg)
{
    state->logger.print(msg);
    state->logger.print("\n");
    state->logger.flush();
}

void log(Logger* logger, std::string_view msg)
{
    logger->print(msg);
    logger->print("\n");
    logger->flush();
}

char** vector_of_strings_to_double_char_array(const std::vector<std::string>& strings)
{
    char** result = static_cast<char**>(malloc(strings.size()));
    int index = 0;
    for (const std::string& string: strings)
    {
        result[index] = const_cast<char*>(string.c_str());
    }
    return result;
}

} // end of namespace
