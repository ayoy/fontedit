#ifndef FONTDATA_H
#define FONTDATA_H

#include <vector>

namespace Font {

struct Size
{
    std::size_t width;
    std::size_t height;
};
inline bool operator==(const Size &lhs, const Size &rhs) noexcept {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}
inline bool operator!=(const Size &lhs, const Size &rhs) noexcept {
    return !(lhs == rhs);
}


struct Point
{
    std::size_t x;
    std::size_t y;

    std::size_t offset(Size sz) { return y * sz.width + x; }
};
inline bool operator==(const Point &lhs, const Point &rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const Point &lhs, const Point &rhs) noexcept {
    return !(lhs == rhs);
}

class Glyph
{
public:
    explicit Glyph(Size sz);
    explicit Glyph(Size sz, std::vector<bool> pixels);

    Size size() const { return size_; }
    bool is_pixel_set(Point p) const { return pixels[p.offset(size_)]; }

    std::vector<bool> pixels;

private:
    Size size_;
};


class RawFaceData
{
public:
    virtual Size font_size() const = 0;
    virtual std::size_t num_glyphs() const = 0;
    virtual bool is_pixel_set(std::size_t glyph_id, Point p) const = 0;

    virtual ~RawFaceData() = default;
};


class Face
{
public:
    Face(const RawFaceData &data);

    std::size_t num_glyphs() const { return glyphs_.size(); }

    Glyph& glyph_at(std::size_t index) { return glyphs_.at(index); }
    const Glyph& glyph_at(std::size_t index) const { return glyphs_.at(index); }

private:
    static std::vector<Glyph> read_glyphs(const RawFaceData &data);
    Size sz_;
    std::vector<Glyph> glyphs_;
};

}



#endif // FONTDATA_H
