#include "gtest/gtest.h"
#include "fontdata.h"

class TestFaceData : public RawFontFaceData
{
public:
    Size font_size() const override { return Size { 4, 3 }; }
    std::size_t num_glyphs() const override { return 5; }

    bool is_pixel_set(std::size_t glyph_id, Point p) const override
    {
        auto glyph_offset = glyph_id * font_size().width * font_size().height;
        return pixels_[glyph_offset + p.offset(font_size())];
    }

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

    for (std::size_t i = 0; i < face.num_glyphs(); i++) {
        auto g = face.glyph_at(i);
        auto g_offset = i * test_data.font_size().width * test_data.font_size().height;
        for (std::size_t y = 0; y < test_data.font_size().height; y++) {
            for (std::size_t x = 0; x < test_data.font_size().width; x++) {
                Point p { x, y };
                auto offset = g_offset + p.offset(test_data.font_size());
                EXPECT_EQ(bool(test_data.pixels_[offset]), g.is_pixel_set(p))
                        << "glyph: " << i << " x: " << x << " y: " << y;
            }
        }
    }

}
