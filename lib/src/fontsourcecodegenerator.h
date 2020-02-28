#ifndef FONTSOURCECODEGENERATOR_H
#define FONTSOURCECODEGENERATOR_H

#include "fontdata.h"
#include "sourcecode.h"

#include <string>
#include <sstream>
#include <bitset>
#include <algorithm>
#include <variant>

static constexpr auto byte_size = 8;

struct Tab {};
struct Space { std::size_t num_spaces; };

using TabWidth = std::variant<Tab,Space>;

struct SourceCodeOptions
{
    enum BitNumbering { LSB, MSB };

    BitNumbering bit_numbering { LSB };
    bool invert_bits { false };
    bool include_line_spacing { false };
    TabWidth tab_width { Tab {} };
};

std::string current_timestamp();
std::string comment_for_glyph(std::size_t index);

/**
 * @brief Converts line margins to pixel margins.
 * @param line_margins - margins expressed in lines
 * @param glyph_size
 * @return Margins expressed in pixel offset for a given glyph size (width).
 */
Font::Margins pixel_margins(Font::Margins line_margins, Font::Size glyph_size);

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
class FontSourceCodeGenerator
{
public:

    FontSourceCodeGenerator(SourceCodeOptions options):
        options_ { options }
    {}

    template<typename T>
    std::string generate(const Font::Face &face, std::string font_name = "font");

private:
    SourceCodeOptions options_;
};

template<typename T>
std::string FontSourceCodeGenerator::generate(const Font::Face &face, std::string font_name)
{
    using namespace SourceCode;

    std::ostringstream s;
    T::append(s, Elem<Idiom::IdiomBegin> { font_name, current_timestamp() });
    T::append(s, Elem<Idiom::IdiomBeginArray> { std::move(font_name) });

    std::bitset<byte_size> bits;
    std::size_t bit_pos { 0 };
    std::size_t col { 0 };
    std::size_t glyph_id { 0 };

    auto width = face.glyph_size().width;

    auto append_byte = [&](std::bitset<byte_size> &bits) {
        if (options_.invert_bits) {
            bits.flip();
        }
        auto byte = static_cast<uint8_t>(bits.to_ulong());
        T::append(s, Elem<Idiom::IdiomByte> { byte });
        bits.reset();
    };


    auto margins = [&] () -> Font::Margins {
        if (options_.include_line_spacing) {
            return {};
        }
        return pixel_margins(face.calculate_margins(), face.glyph_size());
    }();

    for (const auto& glyph : face.glyphs()) {
        T::append(s, Elem<Idiom::IdiomBeginArrayRow> {});

        std::for_each(glyph.pixels().cbegin() + margins.top, glyph.pixels().cend() - margins.bottom,
                      [&](auto pixel) {
            switch (options_.bit_numbering) {
            case SourceCodeOptions::LSB:
                bits[bit_pos] = pixel;
                break;
            case SourceCodeOptions::MSB:
                bits[byte_size-1-bit_pos] = pixel;
                break;
            }

            ++bit_pos;
            ++col;

            if (col >= width) {
                append_byte(bits);
                bit_pos = 0;
                col = 0;
            } else if (bit_pos >= byte_size) {
                append_byte(bits);
                bit_pos = 0;
            }
        });

        T::append(s, Elem<Idiom::IdiomComment> { comment_for_glyph(glyph_id) });
        T::append(s, Elem<Idiom::IdiomLineBreak> {});

        ++glyph_id;
    }

    T::append(s, Elem<Idiom::IdiomEndArray> {});
    T::append(s, Elem<Idiom::IdiomEnd> {});

    return s.str();
}


#endif // FONTSOURCECODEGENERATOR_H
