#ifndef FONTFACEVIEWMODEL_H
#define FONTFACEVIEWMODEL_H

#include <QFont>
#include "f2b.h"
#include <optional>
#include <vector>
#include <exception>
#include <unordered_map>
#include "batchpixelchange.h"

struct FaceInfo
{
    QString fontName;
    Font::Size size;
    Font::Size sizeWithoutMargins;
    std::size_t numberOfGlyphs;
};

class FontFaceViewModel
{
public:
    explicit FontFaceViewModel() = default;
    explicit FontFaceViewModel(Font::Face face, std::optional<QString> name) noexcept;
    explicit FontFaceViewModel(const QFont &font);

    const Font::Face& face() const noexcept { return face_; }
    Font::Face& face() noexcept { return face_; }

    FaceInfo faceInfo() const;

    Font::Face originalFace() const noexcept;
    Font::Margins originalFaceMargins() const noexcept { return originalMargins_; }

    void setActiveGlyphIndex(std::size_t idx) {
        if (idx >= face_.num_glyphs()) {
            throw std::out_of_range("Active glyph index higher than number of glyphs.");
        }
        activeGlyphIndex_ = idx;
    }

    std::optional<std::size_t> activeGlyphIndex() const noexcept {
        return activeGlyphIndex_;
    }

    const Font::Glyph& activeGlyph() const {
        if (!activeGlyphIndex_.has_value()) {
             throw std::logic_error("No active glyph. Call setActiveGlyphIndex() first");
        }
        return face_.glyph_at(activeGlyphIndex_.value());
    }

    void resetActiveGlyph() {
        if (!activeGlyphIndex_.has_value()) {
            return;
        }
        resetGlyph(activeGlyphIndex_.value());
    }

    void reset();

    void modifyGlyph(std::size_t index, const Font::Glyph& new_glyph);

    void modifyGlyph(std::size_t index, const BatchPixelChange &change,
                      BatchPixelChange::ChangeType changeType = BatchPixelChange::ChangeType::Normal);

    bool isModified() const {
        return originalGlyphs_.size() > 0;
    }

    bool isGlyphModified(std::size_t idx) const {
        // modified glyphs have their unmodified counterparts stored in originalGlyphs_.
        return originalGlyphs_.find(idx) != originalGlyphs_.end();
    }

private:
    void resetGlyph(std::size_t idx);
    void doModifyGlyph(std::size_t idx, std::function<void(Font::Glyph&)> change);

    Font::Face face_;
    std::optional<QString> name_;
    Font::Margins originalMargins_;
    // this holds copies of unmodified glyphs once they are edited.
    std::unordered_map<std::size_t, Font::Glyph> originalGlyphs_;

    // not persisted
    std::optional<std::size_t> activeGlyphIndex_;

    friend QDataStream& operator<<(QDataStream&, const FontFaceViewModel&);
    friend QDataStream& operator>>(QDataStream& s, FontFaceViewModel& vm);
};

QDataStream& operator<<(QDataStream& s, const FontFaceViewModel& vm);
QDataStream& operator>>(QDataStream& s, FontFaceViewModel& vm);

#endif // FONTFACEVIEWMODEL_H
