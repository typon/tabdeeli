#include "serde.hpp"

namespace tb
{

String replacement_mode_serialize(ReplacementMode src)
{
    switch (src)
    {
        case ReplacementMode::REGEX: return "REGEX";
        case ReplacementMode::PYTHON: return "PYTHON";
    }

}

} //end namespace tb

