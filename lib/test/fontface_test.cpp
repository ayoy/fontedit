#include "gtest/gtest.h"
#include "fontdata.h"

TEST(fontface, glyphheight)
{
    ASSERT_EQ(Glyph(5, 8).height(), 8);
}
