#ifndef FONTFACEVIEWMODEL_H
#define FONTFACEVIEWMODEL_H

#include <QFont>
#include "f2b.h"
#include <optional>

class FontFaceViewModel
{
public:
    explicit FontFaceViewModel(Font::Face face);
    explicit FontFaceViewModel(const QFont &font);

    const Font::Face& face() const { return face_; }
    Font::Face& face() { return face_; }

    void set_active_glyph_index(std::size_t idx) {
        active_glyph_index_ = idx;
    }

    std::optional<std::size_t> active_glyph_index() const {
        return active_glyph_index_;
    }

    Font::Glyph* active_glyph() {
        if (!active_glyph_index_.has_value()) {
             return nullptr;
        }
        return &face_.glyph_at(active_glyph_index_.value());
    }

private:
    static Font::Face import_face(const QFont &font);

    Font::Face face_;
    std::optional<std::size_t> active_glyph_index_ { std::nullopt };
};

#endif // FONTFACEVIEWMODEL_H
