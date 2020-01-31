#include "gtest/gtest.h"
#include "fontdata.h"

TEST(GlyphTest, API)
{
    Font::Size sz { 5, 5 };

    EXPECT_EQ(sz, Font::Glyph(sz).size());
}
