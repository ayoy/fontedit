#ifndef FONTDATA_H
#define FONTDATA_H

#include <vector>
#include <iostream>
#include <set>

namespace f2b {

namespace font {

/**
 * @brief A struct that describes font top and bottom margins
 *        (lines where no glyphs are drawn).
 */
struct margins {
    std::size_t top;
    std::size_t bottom;
};

inline bool operator==(const margins& lhs, const margins& rhs) noexcept {
    return lhs.top == rhs.top && lhs.bottom == rhs.bottom;
}

inline bool operator!=(const margins& lhs, const margins& rhs) noexcept {
    return !(lhs == rhs);
}


/**
 * @brief A struct that describes font size (glyph size) in pixels.
 */
struct glyph_size
{
    std::size_t width;
    std::size_t height;

    glyph_size with_margins(const margins& m) {
        glyph_size size { width, height };
        auto m_height = m.top + m.bottom;
        if (m_height > height) {
            size.height = 0;
        } else {
            size.height -= m_height;
        }
        return size;
    }
};

inline bool operator==(const glyph_size& lhs, const glyph_size& rhs) noexcept {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}
inline bool operator!=(const glyph_size& lhs, const glyph_size& rhs) noexcept {
    return !(lhs == rhs);
}


/**
 * @brief A struct that describes a point in font glyph coordinates.
 */
struct point
{
    std::size_t x;
    std::size_t y;

    std::size_t offset(glyph_size sz) { return y * sz.width + x; }
};

inline bool operator==(const point& lhs, const point& rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const point& lhs, const point& rhs) noexcept {
    return !(lhs == rhs);
}

/**
 * @brief A class that describes a single Font Glyph.
 *
 * A Glyph is represented by a std::vector<bool> of a fixed size.
 */
class glyph
{
public:
    explicit glyph(glyph_size sz = {});
    explicit glyph(glyph_size sz, std::vector<bool> pixels);

    f2b::font::glyph_size size() const noexcept { return size_; }
    bool is_pixel_set(point p) const { return pixels_[p.offset(size_)]; }
    void set_pixel_set(point p, bool is_set) { pixels_[p.offset(size_)] = is_set; }
    void clear();

    const std::vector<bool>& pixels() const noexcept { return pixels_; }

    std::size_t top_margin() const;
    std::size_t bottom_margin() const;

private:
    font::glyph_size size_;
    std::vector<bool> pixels_;
};

inline bool operator==(const glyph& lhs, const glyph& rhs) noexcept {
    return lhs.size() == rhs.size() && lhs.pixels() == rhs.pixels();
}

inline bool operator!=(const glyph& lhs, const glyph& rhs) noexcept {
    return !(lhs == rhs);
}


/**
 * @brief An abstract class defining an interface for a font face reader.
 *
 * FaceReader instance can be passed as a "data source" for the Face constructor.
 */
class face_reader
{
public:
    virtual glyph_size font_size() const = 0;
    virtual std::size_t num_glyphs() const = 0;
    virtual bool is_pixel_set(std::size_t glyph_id, point p) const = 0;

    virtual ~face_reader() = default;
};


/**
 * @brief A class describing a font face (a set of glyphs for a specific
 *        combination of font family, pixel size and weight).
 */
class face
{
public:

    /// The default constructor intializing an empty face
    explicit face() = default;

    /// The constructor reading a face using face reader. By default all glyphs are exported.
    explicit face(const face_reader &data);

    /// The constructor initializing a face with a given size, vector of glyphs and exported glyph IDs.
    explicit face(glyph_size glyphs_size, std::vector<glyph> glyphs, std::set<std::size_t> exported_glyph_ids = {});

    f2b::font::glyph_size glyphs_size() const noexcept { return sz_; }
    std::size_t num_glyphs() const noexcept { return glyphs_.size(); }

    glyph& glyph_at(std::size_t index) { return glyphs_.at(index); }
    const glyph& glyph_at(std::size_t index) const { return glyphs_.at(index); }

    std::set<std::size_t>& exported_glyph_ids() { return exported_glyph_ids_; }
    const std::set<std::size_t>& exported_glyph_ids() const { return exported_glyph_ids_; }

    const std::vector<glyph>& glyphs() const { return glyphs_; }
    void set_glyph(glyph g, std::size_t index) { glyphs_[index] = g; }
    void append_glyph(glyph g) { glyphs_.push_back(std::move(g)); }
    void delete_last_glyph() { if (glyphs_.size() > 0) glyphs_.pop_back(); }
    void clear_glyph(std::size_t index) {
        if (index >= glyphs_.size()) {
            throw std::out_of_range { "Glyph index out of range" };
        }
        glyphs_[index].clear();
    }

    glyph& operator[](char ascii) {
        if (ascii < ' ') {
            throw std::out_of_range { "Glyphs for 0-31 ASCII range are not supported" };
        }
        return glyphs_[ascii-32];
    }

    const glyph& operator[](char ascii) const {
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
    margins calculate_margins() const noexcept;

private:
    static std::vector<glyph> read_glyphs(const face_reader &data);

    font::glyph_size sz_;
    std::vector<glyph> glyphs_;
    std::set<std::size_t> exported_glyph_ids_;
};

inline bool operator==(const face& lhs, const face& rhs) noexcept {
    return lhs.glyphs_size() == rhs.glyphs_size() && lhs.glyphs() == rhs.glyphs();
}

inline bool operator!=(const face& lhs, const face& rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace font

} // namespace f2b

inline std::ostream& operator<<(std::ostream& os, const f2b::font::glyph& g) {

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
