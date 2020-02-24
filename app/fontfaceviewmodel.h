#ifndef FONTFACEVIEWMODEL_H
#define FONTFACEVIEWMODEL_H

#include <QFont>
#include "f2b.h"
#include <optional>
#include <vector>
#include <exception>
#include <unordered_map>
#include "batchpixelchange.h"

class FontFaceViewModel
{
public:
    explicit FontFaceViewModel(Font::Face face) noexcept;
    explicit FontFaceViewModel(const QFont &font);

    const Font::Face& face() const noexcept { return face_; }
    Font::Face& face() noexcept { return face_; }
    Font::Margins original_face_margins() const noexcept { return original_margins_; }

    Font::Face original_face() const noexcept;

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

    void reset_active_glyph() {
        if (!active_glyph_index_.has_value()) {
            return;
        }
        reset_glyph(active_glyph_index_.value());
    }

    void reset();

    void modify_glyph(std::size_t index, const Font::Glyph& new_glyph);

    void modify_glyph(std::size_t index, const BatchPixelChange &change,
                      BatchPixelChange::ChangeType changeType = BatchPixelChange::ChangeType::Normal);

    bool is_modified() const {
        return originalGlyphs_.size() > 0;
    }

    bool is_glyph_modified(std::size_t idx) const {
        // modified glyphs have their unmodified counterparts stored in originalGlyphs_.
        return originalGlyphs_.find(idx) != originalGlyphs_.end();
    }

private:
    static Font::Face import_face(const QFont &font);
    void reset_glyph(std::size_t idx);
    void do_modify_glyph(std::size_t idx, std::function<void(Font::Glyph&)> change);

    Font::Face face_;
    Font::Margins original_margins_;
    std::optional<std::size_t> active_glyph_index_ { std::nullopt };

    // this holds copies of unmodified glyphs once they are edited.
    std::unordered_map<std::size_t, Font::Glyph> originalGlyphs_;
};

#endif // FONTFACEVIEWMODEL_H
