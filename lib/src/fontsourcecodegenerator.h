#ifndef FONTSOURCECODEGENERATOR_H
#define FONTSOURCECODEGENERATOR_H

#include "fontdata.h"
#include "sourcecode.h"

#include <string>
#include <sstream>

template<typename T>
class FontSourceCodeGenerator
{
public:

    FontSourceCodeGenerator(std::size_t width, std::size_t height, std::size_t num_glyphs, T format):
        width_ { width },
        height_ { height },
        num_glyphs_ { num_glyphs },
        format_ { format }
    {}

    std::string generate(const Font::Face &face);

private:
    std::string get_current_timestamp() const;

    std::size_t width_;
    std::size_t height_;
    std::size_t num_glyphs_;
    T format_;
};

template<typename T>
std::string FontSourceCodeGenerator<T>::get_current_timestamp() const
{
    time_t     now = time(nullptr);
    struct tm  tstruct;
    char       buf[23];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d/%m/%Y at %H:%M:%S", &tstruct);

    return buf;
}

template<typename T>
std::string FontSourceCodeGenerator<T>::generate(const Font::Face &face)
{
    using namespace SourceCode;

    std::ostringstream s;
    format_.append(s, Elem<IdiomBegin> { get_current_timestamp() });
    format_.append(s, Elem<IdiomBegin> { get_current_timestamp() });
    format_.append(s, Elem<IdiomBeginArray> { "font" });

    for (const auto& glyph : face.glyphs()) {
        format_.append(s, Elem<IdiomBeginArrayRow> {});
        format_.append(s, Elem<IdiomByte> { 0xc7 });
        format_.append(s, Elem<IdiomComment> { "pozdrawiam" });
        format_.append(s, Elem<IdiomLineBreak> {});
    }
    format_.append(s, Elem<IdiomEndArray> {});
    format_.append(s, Elem<IdiomEnd> {});

    return s.str();
}

//    m_generator->begin();
//    m_generator->begin_array("font");

//    auto characterCount = 0;

//    for (uint8_t y = 0; y < image.height()/m_height; y++) {
//        for (uint8_t x = 0; x < image.width()/m_width; x++) {
//            m_generator->begin_array_row();
//            for (uint8_t row = 0; row < m_height; row++) {
//                uint8_t remainingBits { m_width };

//                // for width > 8, each line will be represented by more than one byte;
//                // let's track bytes count per line here
//                uint8_t byteIndex {0};

//                while (remainingBits > 0) {
//                    uint8_t byte { 0 };
//                    uint8_t bitCount { std::min(remainingBits, static_cast<uint8_t>(8)) };
//                    uint8_t mask { 1<<7 };
//                    for (uint8_t bit = 0; bit < bitCount; bit++) {
//                        switch (m_readingMode) {
//                        case TopToBottom:
//                            if (image.isPixelSet(x*m_width+bit+8*byteIndex, y*m_height+row)) {
//                                byte |= mask;
//                            }
//                            break;
//                        case LeftToRight:
//                            if (image.isPixelSet(x*m_height+row, y*m_width+bit+8*byteIndex)) {
//                                byte |= mask;
//                            }
//                            break;
//                        }
//                        mask >>= 1;
//                        remainingBits -= 1;
//                    }
//                    m_generator->write_byte(byte);
//                    byteIndex += 1;
//                }
//            }
//            const auto format { "Character 0x%02X (%d)" };
//            auto size = std::snprintf(nullptr, 0, format, characterCount, characterCount);
//            std::string byteString(static_cast<unsigned long>(size), '\0');
//            std::sprintf(&byteString[0], format, characterCount, characterCount);
//            m_generator->add_comment(byteString);
//            m_generator->add_line_break();
//            characterCount += 1;
//        }
//    }
//    m_generator->end_array();
//    m_generator->end();

//    if (error) {
//        *error = ConverterError::NoError;
//    }

//    return m_generator->source_code();
//}


#endif // FONTSOURCECODEGENERATOR_H
