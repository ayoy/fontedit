#include "fontdata.h"

using namespace Font;

Glyph::Glyph(Size sz) :
    size_ { sz },
    pixels_ { std::vector<bool>(sz.width * sz.height, false) }
{}

Glyph::Glyph(Size sz, std::vector<bool> pixels) :
    size_ { sz },
    pixels_ { std::move(pixels) }
{}

Face::Face(const FaceReader &data) :
    sz_ { data.font_size() },
    glyphs_ { read_glyphs(data) }
{}

std::vector<Glyph> Face::read_glyphs(const FaceReader &data)
{
    std::vector<Glyph> glyphs;
    glyphs.reserve(data.num_glyphs());

    const std::vector<bool>::size_type pixels_per_glyph { data.font_size().width * data.font_size().height };

    for (std::size_t i = 0; i < data.num_glyphs(); i++) {

        std::vector<bool> pixels;
        pixels.reserve(pixels_per_glyph);

        for (std::size_t y = 0; y < data.font_size().height; y++) {
            for (std::size_t x = 0; x < data.font_size().width; x++) {
                Point p { x, y };
                pixels.push_back(data.is_pixel_set(i, p));
            }
        }

        Glyph g { data.font_size(), pixels };
        glyphs.push_back(g);
    }

    return glyphs;
}
