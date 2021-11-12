#include "object_utils.hpp"
#include "state.hpp"

namespace tb
{

ByteSlice
byte_slice_from_match(ag_result::ag_match match)
{
    return ByteSlice {.start = match.byte_start, .end = match.byte_end};
}

}


