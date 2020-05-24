#include "gtest/gtest.h"
#include "fontdata.h"

using namespace f2b;

TEST(GlyphTest, API)
{
    font::glyph_size sz { 5, 5 };

    EXPECT_EQ(sz, font::glyph(sz).size());
}
