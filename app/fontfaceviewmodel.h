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
    f2b::font::size size;
    f2b::font::size sizeWithoutMargins;
    std::size_t numberOfGlyphs;
    std::size_t numberOfExportedGlyphs;
};

class FontFaceViewModel
{
public:
    explicit FontFaceViewModel() = default;
    explicit FontFaceViewModel(const QString& documentPath);
    explicit FontFaceViewModel(f2b::font::face face, std::optional<QString> name) noexcept;
    explicit FontFaceViewModel(const QFont& font);

    void saveToFile(const QString& documentPath);

    std::optional<QFont> font() const noexcept { return font_; }

    const f2b::font::face& face() const noexcept { return face_; }
    f2b::font::face& face() noexcept { return face_; }

    FaceInfo faceInfo() const;

    f2b::font::face originalFace() const noexcept;
    f2b::font::margins originalFaceMargins() const noexcept { return originalMargins_; }

    void setGlyphExportedState(std::size_t idx, bool isExported) {
        if (idx >= face_.num_glyphs()) {
            throw std::out_of_range("Active glyph index higher than number of glyphs.");
        }
        if (isExported) {
            face_.exported_glyph_ids().insert(idx);
        } else {
            face_.exported_glyph_ids().erase(idx);
        }
        isDirty_ = true;
    }

    void setActiveGlyphIndex(std::optional<std::size_t> idx) {
        if (idx.has_value() && idx.value() >= face_.num_glyphs()) {
            throw std::out_of_range("Active glyph index higher than number of glyphs.");
        }
        activeGlyphIndex_ = idx;
    }

    std::optional<std::size_t> activeGlyphIndex() const noexcept {
        return activeGlyphIndex_;
    }

    std::optional<f2b::font::glyph> activeGlyph() const {
        if (activeGlyphIndex_.has_value()) {
            return face_.glyph_at(activeGlyphIndex_.value());
        }
        return {};
    }

    void resetActiveGlyph() {
        if (!activeGlyphIndex_.has_value()) {
            return;
        }
        resetGlyph(activeGlyphIndex_.value());
    }

    void reset();

    void resetGlyph(std::size_t index);
    void modifyGlyph(std::size_t index, const f2b::font::glyph& new_glyph);
    void modifyGlyph(std::size_t index, const BatchPixelChange& change,
                      BatchPixelChange::ChangeType changeType = BatchPixelChange::ChangeType::Normal);
    void appendGlyph(f2b::font::glyph newGlyph);
    void deleteGlyph(std::size_t index);

    bool isModified() const {
        return originalGlyphs_.size() > 0;
    }

    bool isGlyphModified(std::size_t idx) const {
        // modified glyphs have their unmodified counterparts stored in originalGlyphs_.
        return originalGlyphs_.count(idx) == 1;
    }

    bool isModifiedSinceSave() const {
        return isDirty_;
    }

private:
    void doModifyGlyph(std::size_t idx, std::function<void(f2b::font::glyph&)> change);

    f2b::font::face face_;
    std::optional<QString> name_;
    f2b::font::margins originalMargins_;
    // this holds copies of unmodified glyphs once they are edited.
    std::unordered_map<std::size_t, f2b::font::glyph> originalGlyphs_;
    std::optional<QFont> font_;

    // not persisted
    std::optional<std::size_t> activeGlyphIndex_;
    bool isDirty_ { false };

    friend QDataStream& operator<<(QDataStream&, const FontFaceViewModel&);
    friend QDataStream& operator>>(QDataStream& s, FontFaceViewModel& vm);
};

QDataStream& operator<<(QDataStream& s, const FontFaceViewModel& vm);
QDataStream& operator>>(QDataStream& s, FontFaceViewModel& vm);

#endif // FONTFACEVIEWMODEL_H
