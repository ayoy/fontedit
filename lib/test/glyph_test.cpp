#include "gtest/gtest.h"
#include "fontdata.h"

using namespace f2b;

TEST(GlyphTest, API)
{
    font::size sz { 5, 5 };

    EXPECT_EQ(sz, font::glyph(sz).size());
}
