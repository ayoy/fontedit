#include "gtest/gtest.h"
#include "fontdata.h"

TEST(GlyphTest, API)
{
    EXPECT_EQ(5, Glyph(Size { 5, 5 }).width());
    EXPECT_EQ(8, Glyph(Size { 5, 8 }).height());
}
