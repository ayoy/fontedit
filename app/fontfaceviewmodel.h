#ifndef FONTFACEVIEWMODEL_H
#define FONTFACEVIEWMODEL_H

#include <QFont>
#include "f2b.h"
#include <optional>
#include <vector>
#include <exception>
#include <unordered_map>

class FontFaceViewModel
{
public:
    explicit FontFaceViewModel(Font::Face face) noexcept;
    explicit FontFaceViewModel(const QFont &font);

    const Font::Face& face() const noexcept { return face_; }
    Font::Face& face() noexcept { return face_; }

    void set_active_glyph_index(std::size_t idx) {
        if (idx >= face_.num_glyphs()) {
            throw std::out_of_range("Active glyph index higher than number of glyphs.");
        }
        active_glyph_index_ = idx;
    }

    std::optional<std::size_t> active_glyph_index() const noexcept {
        return active_glyph_index_;
    }

    const Font::Glyph& active_glyph() const {
        if (!active_glyph_index_.has_value()) {
             throw std::logic_error("No active glyph. Call set_active_glyph_index() first");
        }
        return face_.glyph_at(active_glyph_index_.value());
    }

    /// Copies a glyph to modifiedGlyphs_.
    Font::Glyph& active_glyph();

    void reset_active_glyph() {
        if (!active_glyph_index_.has_value()) {
            return;
        }
        modifiedGlyphs_.erase(active_glyph_index_.value());
    }

    bool is_glyph_modified(std::size_t idx) {
        return modifiedGlyphs_.find(idx) != modifiedGlyphs_.end();
    }

private:
    static Font::Face import_face(const QFont &font);

    Font::Face face_;
    std::unordered_map<std::size_t, Font::Glyph> modifiedGlyphs_;
    std::optional<std::size_t> active_glyph_index_ { std::nullopt };
};

#endif // FONTFACEVIEWMODEL_H
