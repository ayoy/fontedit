#include "fontfaceimporter.h"
#include "f2b.h"
#include "f2b_qt_compat.h"

#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>
#include <QPalette>
#include <QTextDocument>
#include <QTextFrame>

#include <string>


static constexpr std::string_view ascii_glyphs
        { " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" };

static const QString template_text = []{
    std::stringstream stream;
    std::for_each(ascii_glyphs.begin(), ascii_glyphs.end(),
                  [&stream](auto c) { stream << c << "\n"; });

    return QString::fromStdString(stream.str());
}();


class RawQFontAdapter : public Font::RawFaceData
{
public:
    RawQFontAdapter(const QFont &font);
    virtual ~RawQFontAdapter() override = default;

    virtual Font::Size font_size() const override { return sz_; }
    virtual std::size_t num_glyphs() const override { return ascii_glyphs.length(); }
    virtual bool is_pixel_set(std::size_t glyph_id, Font::Point p) const override;

private:
    static std::unique_ptr<QImage> read_font(const QFont &font);

    Font::Size sz_ { 0, 0 };
    std::unique_ptr<QImage> font_image_ { nullptr };
};


RawQFontAdapter::RawQFontAdapter(const QFont &font) :
    Font::RawFaceData(),
    font_image_ { read_font(font) }
{
    QFontMetrics fm(font);
    sz_ = { static_cast<std::size_t>(fm.maxWidth()), static_cast<std::size_t>(fm.lineSpacing()) };
}

bool RawQFontAdapter::is_pixel_set(std::size_t glyph_id, Font::Point p) const
{
    p.y += glyph_id * sz_.height;
    return font_image_->pixelColor(Font::qpoint_with_point(p)) == Qt::color1;
}

std::unique_ptr<QImage> RawQFontAdapter::read_font(const QFont &font)
{
    QFontMetrics fm(font);
//    qDebug() << font << fm.height() << fm.maxWidth() << fm.leading() << fm.lineSpacing();

    auto img_size = QSize(fm.maxWidth(), fm.lineSpacing() * ascii_glyphs.length());
    auto image = std::make_unique<QImage>(img_size, QImage::Format::Format_Mono);

//    qDebug() << "img size" << img_size;

    QPainter p(image.get());
    p.fillRect(QRect(QPoint(), img_size), QColor(Qt::color0));

    QTextDocument doc;
    doc.useDesignMetrics();
    doc.documentLayout()->setPaintDevice(image.get());
    doc.setDefaultFont(font);
    doc.setDocumentMargin(0);
    doc.setPlainText(template_text);
    doc.setTextWidth(img_size.width());

    QTextFrame *rootFrame = doc.rootFrame();
    for (auto it = rootFrame->begin(); !(it.atEnd()); ++it) {
        auto block = it.currentBlock();
        QTextCursor cursor(block);
        auto blockFormat = block.blockFormat();
        blockFormat.setLineHeight(fm.lineSpacing(), QTextBlockFormat::FixedHeight);
        cursor.setBlockFormat(blockFormat);
    }

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, Qt::color1);

    doc.documentLayout()->draw(&p, ctx);

    p.end();

//    image->save("output.bmp");

    return image;
}


FontFaceImporter::FontFaceImporter()
{
}

Font::Face FontFaceImporter::import_face(const QFont &font)
{
    RawQFontAdapter adapter(font);

    return Font::Face(adapter);
}
