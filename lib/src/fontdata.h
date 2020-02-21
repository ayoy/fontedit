#ifndef FONTDATA_H
#define FONTDATA_H

#include <vector>
#include <iostream>

namespace Font {

struct Size
{
    std::size_t width;
    std::size_t height;
};

inline bool operator==(const Size& lhs, const Size& rhs) noexcept {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}
inline bool operator!=(const Size& lhs, const Size& rhs) noexcept {
    return !(lhs == rhs);
}


struct Point
{
    std::size_t x;
    std::size_t y;

    std::size_t offset(Size sz) { return y * sz.width + x; }
};

inline bool operator==(const Point& lhs, const Point& rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const Point& lhs, const Point& rhs) noexcept {
    return !(lhs == rhs);
}


class Glyph
{
public:
    explicit Glyph(Size sz);
    explicit Glyph(Size sz, std::vector<bool> pixels);

    Size size() const noexcept { return size_; }
    bool is_pixel_set(Point p) const { return pixels_[p.offset(size_)]; }
    void set_pixel_set(Point p, bool is_set) { pixels_[p.offset(size_)] = is_set; }

    const std::vector<bool>& pixels() const noexcept { return pixels_; }

private:
    Size size_;
    std::vector<bool> pixels_;
};

inline bool operator==(const Glyph& lhs, const Glyph& rhs) noexcept {
    return lhs.size() == rhs.size() && lhs.pixels() == rhs.pixels();
}

inline bool operator!=(const Glyph& lhs, const Glyph& rhs) noexcept {
    return !(lhs == rhs);
}


class FaceReader
{
public:
    virtual Size font_size() const = 0;
    virtual std::size_t num_glyphs() const = 0;
    virtual bool is_pixel_set(std::size_t glyph_id, Point p) const = 0;

    virtual ~FaceReader() = default;
};


class Face
{
public:
    explicit Face(const FaceReader &data);

    Size glyph_size() const noexcept { return sz_; }
    std::size_t num_glyphs() const noexcept { return glyphs_.size(); }

    Glyph& glyph_at(std::size_t index) { return glyphs_.at(index); }
    const Glyph& glyph_at(std::size_t index) const { return glyphs_.at(index); }

    const std::vector<Glyph>& glyphs() const { return glyphs_; }
    void set_glyph(Glyph g, std::size_t index) { glyphs_[index] = g; }

    Glyph& operator[](char ascii) {
        if (ascii < ' ') {
            throw std::out_of_range { "Glyphs for 0-31 ASCII range are not supported"};
        }
        return glyphs_[ascii-32];
    }

    const Glyph& operator[](char ascii) const {
        if (ascii < ' ') {
            throw std::out_of_range { "Glyphs for 0-31 ASCII range are not supported"};
        }
        return glyphs_[ascii-32];
    }

private:
    static std::vector<Glyph> read_glyphs(const FaceReader &data);

    Size sz_;
    std::vector<Glyph> glyphs_;
};

}

inline std::ostream& operator<<(std::ostream& os, const Font::Glyph& g) {

    std::size_t col = 0;
    for (auto p : g.pixels()) {
        os << p;
        if (++col >= g.size().width) {
            col = 0;
            os << std::endl;
        }
    }
    os << std::flush;

    return os;
}


#endif // FONTDATA_H
