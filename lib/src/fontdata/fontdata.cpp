#include "fontdata.h"

Glyph::Glyph(size_t width, size_t height) :
    width_ { width },
    height_ { height },
    pixels_ { std::vector<bool>(width * height, false) }
{}

Glyph::Glyph(size_t width, size_t height, std::vector<bool> pixels) :
    width_ { width },
    height_ { height },
    pixels_ { std::move(pixels) }
{}
