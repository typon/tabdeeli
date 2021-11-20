#include "stdint.h"
#include <string>

namespace tb
{

using S8          = int8_t;
using S16         = int16_t;
using S32         = int32_t;
using S64         = int64_t;
using U8          = std::byte;
using U16         = uint16_t;
using U32         = uint32_t;
using U64         = uint64_t;
using F32         = float;
using F64         = double;
using B32         = U32;
using PtrSizedInt = uintptr_t;
using Char        = char;
using StringRef   = std::reference_wrapper<const std::string>;
using String      = std::string;

}
