#include "state.hpp"

namespace tb
{

void log(AppState* state, std::string msg)
{
    state->logger.print(msg);
    state->logger.flush();
}

}
