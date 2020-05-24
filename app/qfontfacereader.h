#ifndef QFONTFACEREADER_H
#define QFONTFACEREADER_H

#include "fontdata.h"
#include <QFont>
#include <QImage>
#include <memory>
#include <utility>

class QFontFaceReader : public f2b::Font::FaceReader
{
public:
    explicit QFontFaceReader(const QFont &font, std::string text = {}, std::optional<f2b::Font::Size> forced_size = {});
    virtual ~QFontFaceReader() override = default;

    virtual f2b::Font::Size font_size() const override { return sz_; }
    virtual std::size_t num_glyphs() const override { return num_glyphs_; }
    virtual bool is_pixel_set(std::size_t glyph_id, f2b::Font::Point p) const override;

private:
    static QString template_text(std::string text);
    static std::pair<f2b::Font::Size, std::unique_ptr<QImage>> read_font(
            const QFont &font, std::string text, std::optional<f2b::Font::Size> forcedSize);

    f2b::Font::Size sz_ { 0, 0 };
    std::unique_ptr<QImage> font_image_ { nullptr };
    std::size_t num_glyphs_ { 0 };
};

#endif // QFONTFACEREADER_H
