#include "fontdata.h"

using namespace Font;

Glyph::Glyph(Size sz) :
    size_ { sz },
    pixels_ { std::vector<bool>(sz.width * sz.height, false) }
{}

Glyph::Glyph(Size sz, std::vector<bool> pixels) :
    size_ { sz },
    pixels_ { std::move(pixels) }
{
    if (pixels_.size() != sz.width * sz.height) {
        throw std::logic_error { "pixels size must equal glyph size (width * height)" };
    }
}

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

std::size_t Face::safe_top_margin() const noexcept
{
    std::size_t safe_margin = sz_.height;

//    for (const auto& g : glyphs_) {
//        for (std::size_t i = 0; i < g.pixels().size(); ++i) {
//            if (g.pixels()[i] == true) {
//                auto new_safe_margin = i / sz_.width;
//                if (new_safe_margin < safe_margin) {
//                    safe_margin = new_safe_margin;
//                    break;
//                }
//            }
//        }
//    }


//    for (const auto& g : glyphs_) {
//        auto i = std::find(g.pixels().begin(), g.pixels().end(), true);
//        safe_margin = std::min(safe_margin, std::distance(g.pixels().begin(), i) / sz_.width);
//    }

    std::for_each(glyphs_.begin(), glyphs_.end(), [&](const Font::Glyph& g) {
        auto i = std::find(g.pixels().begin(), g.pixels().end(), true);
        safe_margin = std::min(safe_margin, std::distance(g.pixels().begin(), i) / sz_.width);
    });
    return safe_margin;
}

std::size_t Face::safe_bottom_margin() const noexcept
{
    std::size_t safe_margin = sz_.height;

//    for (const auto& g : glyphs_) {
//        for (std::size_t i = g.pixels().size(); i != 0; --i) {
//            if (g.pixels()[i-1] == true) {
//                auto new_safe_margin = sz_.height - 1 - ((i-1) / sz_.width);
//                if (new_safe_margin < safe_margin) {
//                    safe_margin = new_safe_margin;
//                    break;
//                }
//            }
//        }
//    }


//    for (const auto& g : glyphs_) {
//        auto i = std::find(g.pixels().rbegin(), g.pixels().rend(), true);
//        safe_margin = std::min(safe_margin, std::distance(g.pixels().rbegin(), i) / sz_.width);
//    }

    std::for_each(glyphs_.begin(), glyphs_.end(), [&](const Font::Glyph& g) {
        auto i = std::find(g.pixels().rbegin(), g.pixels().rend(), true);
        safe_margin = std::min(safe_margin, std::distance(g.pixels().rbegin(), i) / sz_.width);
    });

    return safe_margin;
}
