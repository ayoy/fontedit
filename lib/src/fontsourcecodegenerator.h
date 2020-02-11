#ifndef FONTSOURCECODEGENERATOR_H
#define FONTSOURCECODEGENERATOR_H

#include "fontdata.h"
#include "sourcecode.h"

#include <string>
#include <sstream>

/**
 * @brief A simple converter which converts fixed-width fonts.
 *
 * Starting from the top-left 8x8 block, it converts each block from
 * left to right, from top to down.
 *
 * For 'w' width and 'h' height, each w x h block is converted to the total
 * amount of h*(Int(w/8)+1) bytes, i.e. each row is represented by Int(w/8)+1 bytes,
 * so:
 *
 *   width | bytes
 *  ---------------
 *     5   |   1
 *     7   |   1
 *     8   |   1
 *    12   |   2
 *    16   |   2
 *    19   |   3
 *
 * The left bit is the highest bit, and the first byte in a row is a leftmost byte.
 * Unused trailing bits are zeroed.
 * Example:
 * 'A' (17 pixels wide)
 * ...######........ -> 0x1F, 0x80, 0x00
 * ...#######....... -> 0x1F, 0xC0, 0x00
 * .......###....... -> 0x01, 0xC0, 0x00
 * ......##.##...... -> 0x03, 0x60, 0x00
 * ......##.##...... -> 0x03, 0x60, 0x00
 * .....##...##..... -> 0x06, 0x30, 0x00
 * .....##...##..... -> 0x06, 0x30, 0x00
 * ....##....##..... -> 0x0C, 0x30, 0x00
 * ....#########.... -> 0x0F, 0xF8, 0x00
 * ...##########.... -> 0x1F, 0xF8, 0x00
 * ...##.......##... -> 0x18, 0x0C, 0x00
 * ..##........##... -> 0x30, 0x0C, 0x00
 * ######...#######. -> 0xFC, 0x7F, 0x00
 * ######...#######. -> 0xFC, 0x7F, 0x00
 * ................. -> 0x00, 0x00, 0x00
 *
 * This char will result in the byte sequence: 0x1F, 0x80, 0x00, 0x1F, 0xC0, 0x00, ...
 *
 * '9' (8 pixels wide)
 * ..XXXX.. -> 0x3C
 * .XX..XX. -> 0x66
 * .XX..XX. -> 0x66
 * ..XXXXX. -> 0x3E
 * .....XX. -> 0x06
 * .....XX. -> 0x06
 * .XX..XX. -> 0x66
 * ..XXXX.. -> 0x3C
 * ........ -> 0x00
 *
 * This char will result in the byte sequence: 0x3c, 0x66, 0x66, ...
 *
 */
template<typename T>
class FontSourceCodeGenerator
{
public:

    FontSourceCodeGenerator(std::size_t width, std::size_t height, std::size_t num_glyphs):
        width_ { width },
        height_ { height },
        num_glyphs_ { num_glyphs },
        format_ { T {} }
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
