#include "gtest/gtest.h"
#include "fontdata.h"

using namespace f2b;

TEST(GlyphTest, API)
{
    Font::Size sz { 5, 5 };

    EXPECT_EQ(sz, Font::Glyph(sz).size());
}
