#ifndef GENERAL_STANDARD_TYPES_H
#define GENERAL_STANDARD_TYPES_H

#include <memory>

namespace General
{
    namespace Type
    {
        typedef int             Int;
        typedef unsigned int    Uint;
        typedef uint8_t         Uint8;
        typedef int32_t         Int32;
        typedef uint16_t        Uint16;
        typedef uint32_t        Uint32;
        typedef uint64_t        Uint64;
        typedef char            Char;
        typedef unsigned char   UChar;
        typedef double          Double;
        typedef float           Float;
    }

    namespace Pointer 
    {
        template<typename T>
        using Scope = std::unique_ptr<T>;

        template<typename T>
        using Shared = std::shared_ptr<T>;
    }
}

#endif
