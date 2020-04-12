#include "fontfaceviewmodel.h"
#include "f2b.h"
#include "f2b_qt_compat.h"
#include "qfontfacereader.h"

#include <utility>
#include <stdexcept>
#include <cassert>

#include <QDebug>
#include <QPalette>
#include <QFileInfo>


Font::Face import_face(const QFont &font)
{
    QFontFaceReader adapter(font);
    return Font::Face(adapter);
}

QString font_name(const QFont &font)
{
    QStringList s;
    s << QString("%1 %2pt").arg(font.family()).arg(QString::number(font.pointSize()));
    if (font.bold()) {
        s << "Bold";
    }
    if (font.italic()) {
        s << "Italic";
    }
    if (font.underline()) {
        s << "Underline";
    }
    if (font.strikeOut()) {
        s << "Strikeout";
    }

    return s.join(", ");
}


FontFaceViewModel::FontFaceViewModel(const QString& documentFilePath)
{
    QFile f(documentFilePath);
    if (!f.exists() || !f.permissions().testFlag(QFileDevice::ReadUser)) {
        throw std::runtime_error { "Unable to open file " + documentFilePath.toStdString() };
    }

    f.open(QIODevice::ReadOnly);
    QDataStream s(&f);
    s >> *this;
    f.close();
    isDirty_ = false;
}

FontFaceViewModel::FontFaceViewModel(Font::Face face, std::optional<QString> name) noexcept :
    face_ { face },
    name_ { name },
    originalMargins_ { face.calculate_margins() }
{
}

FontFaceViewModel::FontFaceViewModel(const QFont &font) :
    FontFaceViewModel(import_face(font), font_name(font))
{
    font_ = font;
    isDirty_ = true;
}

void FontFaceViewModel::saveToFile(const QString &documentPath)
{
    QFile f(documentPath);
    QFile directory(QFileInfo(documentPath).path());

    if (!directory.permissions().testFlag(QFileDevice::WriteUser) ||
            (f.exists() && !f.permissions().testFlag(QFileDevice::WriteUser)))
    {
        throw std::runtime_error { "Unable to write to file: " + documentPath.toStdString() };
    }

    f.open(QIODevice::WriteOnly);
    QDataStream s(&f);
    s << *this;
    f.close();
    isDirty_ = false;
}

FaceInfo FontFaceViewModel::faceInfo() const
{
    auto fontName = name_.has_value() ? name_.value() : QObject::tr("Custom font");
    auto size = face_.glyph_size();
    size.height -= originalMargins_.top + originalMargins_.bottom;
    return { fontName, face_.glyph_size(), size, face_.num_glyphs(), face_.exported_glyph_ids().size() };
}

void FontFaceViewModel::modifyGlyph(std::size_t index, const Font::Glyph &new_glyph)
{
    doModifyGlyph(index, [&](Font::Glyph &glyph) {
        glyph = new_glyph;
    });
}

void FontFaceViewModel::modifyGlyph(std::size_t index,
                                     const BatchPixelChange &change,
                                     BatchPixelChange::ChangeType changeType)
{
    doModifyGlyph(index, [&](Font::Glyph& glyph) {
        change.apply(glyph, changeType);
    });
}

void FontFaceViewModel::doModifyGlyph(std::size_t idx, std::function<void (Font::Glyph&)> change)
{
    Font::Glyph& glyph { face_.glyph_at(idx) };
    bool first_change = false;

    if (originalGlyphs_.count(idx) == 0) {
        qDebug() << "active_glyph non-const cache miss";
        originalGlyphs_.insert({ idx, glyph });
        first_change = true;
    } else {
        qDebug() << "active_glyph non-const cache hit";
    }

    change(glyph);
    isDirty_ = true;

    // remove glyph from originals when restoring initial state
    if (!first_change && glyph == originalGlyphs_.at(idx)) {
        originalGlyphs_.erase(idx);
    }
}

void FontFaceViewModel::reset()
{
    face_ = originalFace();
    originalGlyphs_.clear();
}

void FontFaceViewModel::resetGlyph(std::size_t index)
{
    if (isGlyphModified(index)) {
        face_.set_glyph(originalGlyphs_.at(index), index);
        originalGlyphs_.erase(activeGlyphIndex_.value());
        isDirty_ = true;
    }
}

void FontFaceViewModel::appendGlyph(Font::Glyph newGlyph)
{
    face_.append_glyph(std::move(newGlyph));
    isDirty_ = true;
}

void FontFaceViewModel::deleteGlyph(std::size_t index)
{
    if (index == face_.num_glyphs() - 1) {
        if (activeGlyphIndex_.has_value() && activeGlyphIndex_.value() == face_.num_glyphs() - 1) {
            activeGlyphIndex_ = face_.num_glyphs() - 2;
        }
        face_.delete_last_glyph();
    } else {
        face_.clear_glyph(index);
    }
    isDirty_ = true;
}

Font::Face FontFaceViewModel::originalFace() const noexcept
{
    Font::Face f = face_;
    for (const auto& pair : originalGlyphs_) {
        f.set_glyph(pair.second, pair.first);
    }
    return f;
}

static constexpr auto fontfaceviewmodel_magic_number = 0x1c22f998;
static constexpr auto fontfaceviewmodel_version = 2;

QDataStream& operator<<(QDataStream& s, const FontFaceViewModel &vm)
{
    s << (quint32) fontfaceviewmodel_magic_number;
    s << (qint32) fontfaceviewmodel_version;
    s.setVersion(QDataStream::Qt_5_7);

    s << vm.face_;
    s << vm.name_;
    s << (quint32) vm.originalMargins_.top << (quint32) vm.originalMargins_.bottom;

    s << vm.font_;
    return s;
}

QDataStream& operator>>(QDataStream& s, FontFaceViewModel& vm)
{
    quint32 magic_number;
    quint32 version;
    s >> magic_number >> version;
    if (magic_number == fontfaceviewmodel_magic_number && version <= fontfaceviewmodel_version) {
        s.setVersion(QDataStream::Qt_5_7);

        s >> vm.face_;
        s >> vm.name_;

        quint32 top, bottom;
        s >> top >> bottom;
        vm.originalMargins_ = { top, bottom };
        vm.originalGlyphs_ = {};
        vm.activeGlyphIndex_ = {};

        if (version <= 2) {
            s >> vm.font_;
        }
    }

    return s;
}
