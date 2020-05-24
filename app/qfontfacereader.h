#ifndef QFONTFACEREADER_H
#define QFONTFACEREADER_H

#include "fontdata.h"
#include <QFont>
#include <QImage>
#include <memory>
#include <utility>

class QFontFaceReader : public f2b::font::face_reader
{
public:
    explicit QFontFaceReader(const QFont &font, std::string text = {}, std::optional<f2b::font::glyph_size> forced_size = {});
    virtual ~QFontFaceReader() override = default;

    virtual f2b::font::glyph_size font_size() const override { return sz_; }
    virtual std::size_t num_glyphs() const override { return num_glyphs_; }
    virtual bool is_pixel_set(std::size_t glyph_id, f2b::font::point p) const override;

private:
    static QString template_text(std::string text);
    static std::pair<f2b::font::glyph_size, std::unique_ptr<QImage>> read_font(
            const QFont &font, std::string text, std::optional<f2b::font::glyph_size> forcedSize);

    f2b::font::glyph_size sz_ { 0, 0 };
    std::unique_ptr<QImage> font_image_ { nullptr };
    std::size_t num_glyphs_ { 0 };
};

#endif // QFONTFACEREADER_H
