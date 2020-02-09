#include "gtest/gtest.h"
#include "sourcecode.h"

using namespace SourceCode;

static_assert (needs_string_arg<IdiomBegin>::value, "");
static_assert (needs_string_arg<IdiomBeginArray>::value, "");
static_assert (!needs_string_arg<IdiomBeginArrayRow>::value, "");
static_assert (!needs_string_arg<IdiomByte>::value, "");
static_assert (needs_string_arg<IdiomComment>::value, "");
static_assert (!needs_string_arg<IdiomLineBreak>::value, "");
static_assert (!needs_string_arg<IdiomEndArray>::value, "");
static_assert (!needs_string_arg<IdiomEnd>::value, "");

static_assert (!needs_byte_arg<IdiomBegin>::value, "");
static_assert (!needs_byte_arg<IdiomBeginArray>::value, "");
static_assert (!needs_byte_arg<IdiomBeginArrayRow>::value, "");
static_assert (needs_byte_arg<IdiomByte>::value, "");
static_assert (!needs_byte_arg<IdiomComment>::value, "");
static_assert (!needs_byte_arg<IdiomLineBreak>::value, "");
static_assert (!needs_byte_arg<IdiomEndArray>::value, "");
static_assert (!needs_byte_arg<IdiomEnd>::value, "");

TEST(SourceCodeTest, IdiomTraits)
{
}
