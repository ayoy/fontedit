#ifndef FONTDATA_H
#define FONTDATA_H

#include <vector>
#include <iostream>
#include <set>

namespace Font {

/**
 * @brief A struct that describes font top and bottom margins
 *        (lines where no glyphs are drawn).
 */
struct Margins {
    std::size_t top;
    std::size_t bottom;
};

inline bool operator==(const Margins& lhs, const Margins& rhs) noexcept {
    return lhs.top == rhs.top && lhs.bottom == rhs.bottom;
}

inline bool operator!=(const Margins& lhs, const Margins& rhs) noexcept {
    return !(lhs == rhs);
}


/**
 * @brief A struct that describes font size (glyph size) in pixels.
 */
struct Size
{
    std::size_t width;
    std::size_t height;

    Size with_margins(const Margins& m) {
        Size size { width, height };
        auto m_height = m.top + m.bottom;
        if (m_height > height) {
            size.height = 0;
        } else {
            size.height -= m_height;
        }
        return size;
    }
};

inline bool operator==(const Size& lhs, const Size& rhs) noexcept {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}
inline bool operator!=(const Size& lhs, const Size& rhs) noexcept {
    return !(lhs == rhs);
}


/**
 * @brief A struct that describes a point in font glyph coordinates.
 */
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

/**
 * @brief A class that describes a single Font Glyph.
 *
 * A Glyph is represented by a std::vector<bool> of a fixed size.
 */
class Glyph
{
public:
    explicit Glyph(Size sz = {});
    explicit Glyph(Size sz, std::vector<bool> pixels);

    Size size() const noexcept { return size_; }
    bool is_pixel_set(Point p) const { return pixels_[p.offset(size_)]; }
    void set_pixel_set(Point p, bool is_set) { pixels_[p.offset(size_)] = is_set; }
    void clear();

    const std::vector<bool>& pixels() const noexcept { return pixels_; }

    std::size_t top_margin() const;
    std::size_t bottom_margin() const;

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


/**
 * @brief An abstract class defining an interface for a font face reader.
 *
 * FaceReader instance can be passed as a "data source" for the Face constructor.
 */
class FaceReader
{
public:
    virtual Size font_size() const = 0;
    virtual std::size_t num_glyphs() const = 0;
    virtual bool is_pixel_set(std::size_t glyph_id, Point p) const = 0;

    virtual ~FaceReader() = default;
};


/**
 * @brief A class describing a font face (a set of glyphs for a specific
 *        combination of font family, pixel size and weight).
 */
class Face
{
public:

    /// The default constructor intializing an empty face
    explicit Face() = default;

    /// The constructor reading a face using face reader. By default all glyphs are exported.
    explicit Face(const FaceReader &data);

    /// The constructor initializing a face with a given size, vector of glyphs and exported glyph IDs.
    explicit Face(Size size, std::vector<Glyph> glyphs, std::set<std::size_t> exported_glyph_ids = {});

    Size glyph_size() const noexcept { return sz_; }
    std::size_t num_glyphs() const noexcept { return glyphs_.size(); }

    Glyph& glyph_at(std::size_t index) { return glyphs_.at(index); }
    const Glyph& glyph_at(std::size_t index) const { return glyphs_.at(index); }

    std::set<std::size_t>& exported_glyph_ids() { return exported_glyph_ids_; }
    const std::set<std::size_t>& exported_glyph_ids() const { return exported_glyph_ids_; }

    const std::vector<Glyph>& glyphs() const { return glyphs_; }
    void set_glyph(Glyph g, std::size_t index) { glyphs_[index] = g; }
    void append_glyph(Glyph g) { glyphs_.push_back(std::move(g)); }
    void delete_last_glyph() { if (glyphs_.size() > 0) glyphs_.pop_back(); }
    void clear_glyph(std::size_t index) {
        if (index >= glyphs_.size()) {
            throw std::out_of_range { "Glyph index out of range" };
        }
        glyphs_[index].clear();
    }

    Glyph& operator[](char ascii) {
        if (ascii < ' ') {
            throw std::out_of_range { "Glyphs for 0-31 ASCII range are not supported" };
        }
        return glyphs_[ascii-32];
    }

    const Glyph& operator[](char ascii) const {
        if (ascii < ' ') {
            throw std::out_of_range { "Glyphs for 0-31 ASCII range are not supported" };
        }
        return glyphs_[ascii-32];
    }

    /**
     * Calculcates margins of a face.
     *
     * This method analyzes all glyphs and takes the maximum common empty areas
     * on the top and bottom.
     */
    Margins calculate_margins() const noexcept;

private:
    static std::vector<Glyph> read_glyphs(const FaceReader &data);

    Size sz_;
    std::vector<Glyph> glyphs_;
    std::set<std::size_t> exported_glyph_ids_;
};

inline bool operator==(const Face& lhs, const Face& rhs) noexcept {
    return lhs.glyph_size() == rhs.glyph_size() && lhs.glyphs() == rhs.glyphs();
}

inline bool operator!=(const Face& lhs, const Face& rhs) noexcept {
    return !(lhs == rhs);
}

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
