#ifndef FONTDATA_H
#define FONTDATA_H

#include <vector>
#include <optional>
#include <sstream>

class Glyph {

public:
    explicit Glyph(size_t width, size_t height);
    explicit Glyph(size_t width, size_t height, std::vector<bool> pixels);

    size_t width() const { return width_; }
    size_t height() const { return height_; }

private:
    size_t width_;
    size_t height_;
    std::vector<bool> pixels_;
};


class FontFace
{
public:
    FontFace(std::istringstream &bitmap_data);

private:
    std::vector<Glyph> glyphs_;
};


#endif // FONTDATA_H
