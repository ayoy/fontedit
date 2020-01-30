#ifndef FONTDATA_H
#define FONTDATA_H

#include <vector>

struct Point {
    std::size_t x;
    std::size_t y;
};

struct Size {
    std::size_t width;
    std::size_t height;
};

class Glyph {

public:
    explicit Glyph(Size sz);
    Glyph(Size sz, std::vector<bool> pixels);

    std::size_t width() const { return size_.width; }
    std::size_t height() const { return size_.height; }

    bool is_pixel_set(Point p) const {
        return bool(pixels_[p.y * size_.width + p.x]);
    }

private:
    Size size_;
    std::vector<bool> pixels_;
};


class RawFontFaceData
{
public:
    virtual Size font_size() const = 0;
    virtual std::size_t num_glyphs() const = 0;
    virtual bool is_pixel_set(std::size_t glyph_id, Point p) const = 0;

    virtual ~RawFontFaceData() = default;
};


class FontFace
{
public:
    FontFace(const RawFontFaceData &data);

    std::size_t num_glyphs() const { return glyphs_.size(); }

    Glyph& glyph_at(std::size_t index) { return glyphs_.at(index); }
    const Glyph& glyph_at(std::size_t index) const { return glyphs_.at(index); }

private:
    static std::vector<Glyph> read_glyphs(const RawFontFaceData &data);
    Size sz_;
    std::vector<Glyph> glyphs_;
};


#endif // FONTDATA_H
