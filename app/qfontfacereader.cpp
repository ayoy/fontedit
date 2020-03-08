#include "qfontfacereader.h"
#include "f2b_qt_compat.h"

#include <QAbstractTextDocumentLayout>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>
#include <QTextDocument>
#include <QTextFrame>
#include "utf8.h"

using namespace std::literals::string_view_literals;

static constexpr std::string_view ascii_glyphs =
        " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"sv;

QFontFaceReader::QFontFaceReader(const QFont &font, std::string text) :
    Font::FaceReader()
{
    std::string source_text { text.empty() ? ascii_glyphs : std::move(text) };
    num_glyphs_ = source_text.length();

    auto result = read_font(font, std::move(source_text));
    sz_ = result.first;
    font_image_ = std::move(result.second);
}

bool QFontFaceReader::is_pixel_set(std::size_t glyph_id, Font::Point p) const
{
    p.y += glyph_id * sz_.height;
    return font_image_->pixelColor(Font::qpoint_with_point(p)) == Qt::color1;
}

QString QFontFaceReader::template_text(std::string text)
{
    std::stringstream stream;

    utf8::iterator i(text.begin(), text.begin(), text.end());
    utf8::iterator end(text.end(), text.begin(), text.end());

    while (i != end) {
        stream << std::string(i.base(), (++i).base()) << "\n";
    }

    return QString::fromStdString(stream.str());
}

std::pair<Font::Size, std::unique_ptr<QImage>> QFontFaceReader::read_font(const QFont &font, std::string text)
{
    auto text_length = text.length();
    auto template_text = QFontFaceReader::template_text(std::move(text));

    QFontMetrics fm(font);
    qDebug() << font << fm.height() << fm.maxWidth() << fm.leading() << fm.lineSpacing();

    auto width = fm.boundingRect(QRect(), Qt::AlignLeft, template_text).width();
    QSize img_size(width, fm.lineSpacing() * text_length);

//    qDebug() << "img size" << img_size;

    auto image = std::make_unique<QImage>(img_size, QImage::Format::Format_Mono);
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

    return { Font::size_with_qsize(QSize(width, fm.lineSpacing())), std::move(image) };
}

