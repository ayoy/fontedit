#ifndef SOURCECODE_H
#define SOURCECODE_H

#include <string>

namespace SourceCode
{

enum class Idiom {
    IdiomBegin,
    IdiomBeginArray,
    IdiomBeginArrayRow,
    IdiomByte,
    IdiomComment,
    IdiomLineBreak,
    IdiomEndArray,
    IdiomEnd
};

template<Idiom I, typename = void>
struct needs_byte_arg : std::false_type {};
template<Idiom I>
struct needs_byte_arg<I, std::enable_if_t<(I == Idiom::IdiomByte)>> : std::true_type {};

template<Idiom I, typename = void>
struct needs_string_arg : std::false_type {};
template<Idiom I>
struct needs_string_arg<I, std::enable_if_t<(
        I == Idiom::IdiomBegin || I == Idiom::IdiomBeginArray || I == Idiom::IdiomComment
        )>> : std::true_type {};


template<Idiom I, typename Enable = void>
struct Elem
{
};

template<Idiom I>
struct Elem<I, typename std::enable_if_t<(!needs_byte_arg<I>::value && !needs_string_arg<I>::value)>>
{
};

template<Idiom I>
struct Elem<I, typename std::enable_if_t<needs_byte_arg<I>::value>>
{
    uint8_t byte;

    Elem(uint8_t byte) : byte { byte } {};
};

template<Idiom I>
struct Elem<I, typename std::enable_if_t<needs_string_arg<I>::value>>
{
    std::string arg;

    Elem(const std::string& arg) : arg { arg } {};
};

namespace Element {

template<Idiom I>
using Bare = struct Elem<I, typename std::enable_if_t<(!needs_byte_arg<I>::value && !needs_string_arg<I>::value)>>;

template<Idiom I>
using Byte = struct Elem<I, typename std::enable_if_t<needs_byte_arg<I>::value>>;

template<Idiom I>
using String = struct Elem<I, typename std::enable_if_t<needs_string_arg<I>::value>>;
}

}

#endif // SOURCECODE_H
