#include "gtest/gtest.h"
#include "fontdata.h"

class TestFaceData : public RawFontFaceData
{
    Size font_size() const override { return Size { 4, 3 }; }
    std::size_t num_glyphs() const override { return 5; }

    bool is_pixel_set(std::size_t glyph_id, Point p) const override
    {
        auto glyph_offset = glyph_id * font_size().width * font_size().height;
        return pixels_[glyph_offset + p.y * font_size().width + p.x];
    }

private:
    std::vector<bool> pixels_
    {
        0, 0, 0, 0,
        0, 0, 0, 1,
        0, 0, 1, 0,
        //
        0, 1, 0, 0,
        1, 0, 1, 0,
        1, 0, 0, 1,
        //
        1, 1, 1, 1,
        1, 1, 0, 1,
        1, 0, 1, 1,
        //
        1, 0, 0, 1,
        1, 0, 1, 0,
        0, 0, 1, 1,
        //
        0, 1, 1, 1,
        1, 1, 0, 0,
        1, 0, 1, 0
    };
};


TEST(FontFaceTest, Initialization)
{
    TestFaceData test_data;
    FontFace face(test_data);

    EXPECT_EQ(5, face.num_glyphs());

    auto g = face.glyph_at(2);
    EXPECT_EQ(4, g.width());
    EXPECT_EQ(3, g.height());

    EXPECT_EQ(true, g.is_pixel_set(Point { 0, 0 }));
    EXPECT_EQ(true, g.is_pixel_set(Point { 1, 0 }));
    EXPECT_EQ(true, g.is_pixel_set(Point { 2, 0 }));
    EXPECT_EQ(true, g.is_pixel_set(Point { 3, 0 }));
    EXPECT_EQ(true, g.is_pixel_set(Point { 0, 1 }));
    EXPECT_EQ(true, g.is_pixel_set(Point { 1, 1 }));
    EXPECT_EQ(false, g.is_pixel_set(Point { 2, 1 }));
    EXPECT_EQ(true, g.is_pixel_set(Point { 3, 1 }));
    EXPECT_EQ(true, g.is_pixel_set(Point { 0, 2 }));
    EXPECT_EQ(false, g.is_pixel_set(Point { 1, 2 }));
    EXPECT_EQ(true, g.is_pixel_set(Point { 2, 2 }));
    EXPECT_EQ(true, g.is_pixel_set(Point { 3, 2 }));
}
