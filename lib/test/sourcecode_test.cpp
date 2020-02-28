#include "gtest/gtest.h"
#include "sourcecode.h"

using namespace SourceCode;

static_assert (needs_two_string_args<Idiom::IdiomBegin>::value, "");
static_assert (needs_string_arg<Idiom::IdiomBeginArray>::value, "");
static_assert (!needs_string_arg<Idiom::IdiomBeginArrayRow>::value, "");
static_assert (!needs_string_arg<Idiom::IdiomByte>::value, "");
static_assert (needs_string_arg<Idiom::IdiomComment>::value, "");
static_assert (!needs_string_arg<Idiom::IdiomLineBreak>::value, "");
static_assert (!needs_string_arg<Idiom::IdiomEndArray>::value, "");
static_assert (!needs_string_arg<Idiom::IdiomEnd>::value, "");

static_assert (!needs_byte_arg<Idiom::IdiomBegin>::value, "");
static_assert (!needs_string_arg<Idiom::IdiomBegin>::value, "");
static_assert (!needs_byte_arg<Idiom::IdiomBeginArray>::value, "");
static_assert (!needs_byte_arg<Idiom::IdiomBeginArrayRow>::value, "");
static_assert (needs_byte_arg<Idiom::IdiomByte>::value, "");
static_assert (!needs_byte_arg<Idiom::IdiomComment>::value, "");
static_assert (!needs_byte_arg<Idiom::IdiomLineBreak>::value, "");
static_assert (!needs_byte_arg<Idiom::IdiomEndArray>::value, "");
static_assert (!needs_byte_arg<Idiom::IdiomEnd>::value, "");

TEST(SourceCodeTest, IdiomTraits)
{
}
