#include "gtest/gtest.h"
#include "fontdata.h"

TEST(glyph, glyphwidth)
{
    ASSERT_EQ(Glyph(5, 5).width(), 5);
}
