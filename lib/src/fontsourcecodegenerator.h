#ifndef FONTSOURCECODEGENERATOR_H
#define FONTSOURCECODEGENERATOR_H

#include "fontdata.h"
#include "sourcecode.h"
#include "format.h"

#include <string>
#include <sstream>
#include <bitset>
#include <algorithm>

static constexpr auto byte_size = 8;

struct SourceCodeOptions
{
    enum BitNumbering { LSB, MSB };
    enum ExportMethod { ExportSelected, ExportAll };

    ExportMethod export_method { ExportSelected };
    BitNumbering bit_numbering { LSB };
    bool invert_bits { false };
    bool include_line_spacing { false };
    SourceCode::Indentation indentation { SourceCode::Tab {} };
};

/**
 * @brief Converts line margins to pixel margins.
 * @param line_margins - margins expressed in lines
 * @param glyph_size
 * @return Margins expressed in pixel offset for a given glyph size (width).
 */
Font::Margins pixel_margins(Font::Margins line_margins, Font::Size glyph_size);


class FontSourceCodeGeneratorInterface
{
public:
    virtual std::string current_timestamp() = 0;
    virtual std::string comment_for_glyph(std::size_t index) = 0;
};

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
class FontSourceCodeGenerator : private FontSourceCodeGeneratorInterface
{
public:

    FontSourceCodeGenerator(SourceCodeOptions options):
        options_ { options }
    {}

    /**
     * A template method that generates source code for a given \c face.
     *
     * The source code format is defined as a template parameter,
     * and the optional \c font_name parameter allows for customising
     * the font byte array parameter.
     *
     * The generator builds the source code out of building blocks -
     * structs from \c SourceCode::Idiom namespace.
     *
     * @see SourceCode::Idiom
     */
    template<typename T>
    std::string generate(const Font::Face& face, std::string font_name = "font");

private:
    template<typename T>
    std::string generate_all(const Font::Face& face, std::string font_name = "font");

    template<typename T>
    std::string generate_subset(const Font::Face& face, std::string font_name = "font");

    template<typename T, typename V>
    std::string subset_lut(const std::set<std::size_t>& exported_glyph_ids,
                           std::size_t bytes_per_glyph);

    std::string current_timestamp() override;
    std::string comment_for_glyph(std::size_t index) override;
    SourceCodeOptions options_;
};

template<typename T>
std::string FontSourceCodeGenerator::generate_all(const Font::Face& face, std::string font_name)
{
    using namespace SourceCode;

    auto [size, margins] = [&] () -> std::pair<Font::Size, Font::Margins> {
        if (options_.include_line_spacing) {
            return { face.glyph_size(), {} };
        }
        auto line_margins = face.calculate_margins();
        return { face.glyph_size().with_margins(line_margins),
                    pixel_margins(line_margins, face.glyph_size()) };
    }();

    std::ostringstream s;
    s << Idiom::Begin<T> { font_name, size, current_timestamp() };
    s << Idiom::BeginArray<T, uint8_t> { std::move(font_name) };

    std::bitset<byte_size> bits;
    std::size_t bit_pos { 0 };
    std::size_t col { 0 };
    std::size_t glyph_id { 0 };

    auto append_byte = [&](std::bitset<byte_size> &bits) {
        if (options_.invert_bits) {
            bits.flip();
        }
        auto byte = static_cast<uint8_t>(bits.to_ulong());
        s << Idiom::Value<T, uint8_t> { std::move(byte) };
        bits.reset();
    };

    auto width = size.width;

    for (const auto& glyph : face.glyphs()) {
        s << Idiom::BeginArrayRow<T, uint8_t> { options_.indentation };

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

        s << Idiom::Comment<T, uint8_t> { comment_for_glyph(glyph_id) };
        s << Idiom::ArrayLineBreak<T, uint8_t> {};

        ++glyph_id;
    }

    s << Idiom::EndArray<T, uint8_t> {};
    s << Idiom::End<T> {};

    return s.str();
}

template<typename T, typename V>
std::string FontSourceCodeGenerator::subset_lut(const std::set<std::size_t>& exported_glyph_ids,
                                                std::size_t bytes_per_glyph)
{
    using namespace SourceCode;

    std::ostringstream s;

    uint16_t exported_id { 0 };

    auto last_exported_glyph = std::prev(exported_glyph_ids.end());

    s << Idiom::BeginArray<T, V> { "lut" };

    bool is_previous_exported = true;
    for (std::size_t glyph_id = 0; glyph_id <= *last_exported_glyph; ++glyph_id) {
        if (exported_glyph_ids.find(glyph_id) != exported_glyph_ids.end()) {
            if (!is_previous_exported)
                s << Idiom::ArrayLineBreak<T> {};
            s << Idiom::BeginArrayRow<T, V> { options_.indentation };
            s << Idiom::Value<T, V> { static_cast<V>(bytes_per_glyph * exported_id) };
            s << Idiom::Comment<T> { comment_for_glyph(glyph_id) };
            ++exported_id;
            s << Idiom::ArrayLineBreak<T> {};
            is_previous_exported = true;
        } else {
            if (is_previous_exported)
                s << Idiom::BeginArrayRow<T, V> { options_.indentation };
            s << Idiom::Value<T, V> { 0 };
            is_previous_exported = false;
        }
    }

    s << Idiom::EndArray<T, V> {};

    return s.str();
}

/**
 * begin_document
 * begin_array<byte>(data)
 *     begin_array_row(), byte(x), comment(...), line_break()
 * end_array()
 * begin_array<int>(font)
 *     begin_array_row(), value(x), comment(...), line_break()
 * end_array()
 */
template<typename T>
std::string FontSourceCodeGenerator::generate_subset(const Font::Face& face, std::string font_name)
{
    using namespace SourceCode;

    auto [size, margins] = [&] () -> std::pair<Font::Size, Font::Margins> {
        if (options_.include_line_spacing) {
            return { face.glyph_size(), {} };
        }
        auto line_margins = face.calculate_margins();
        return { face.glyph_size().with_margins(line_margins),
                    pixel_margins(line_margins, face.glyph_size()) };
    }();

    std::ostringstream s;
    s << Idiom::Begin<T> { font_name, size, current_timestamp() };
    s << Idiom::BeginArray<T, uint8_t> { std::move(font_name) };

    std::bitset<byte_size> bits;
    std::size_t bit_pos { 0 };
    std::size_t col { 0 };

    auto append_byte = [&](std::bitset<byte_size> &bits) {
        if (options_.invert_bits) {
            bits.flip();
        }
        auto byte = static_cast<uint8_t>(bits.to_ulong());
        s << Idiom::Value<T, uint8_t> { std::move(byte) };
        bits.reset();
    };

    auto width = size.width;

    for (auto glyph_id : face.exported_glyph_ids()) {
        const auto& glyph = face.glyph_at(glyph_id);
        s << Idiom::BeginArrayRow<T, uint8_t> { options_.indentation };

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

        s << Idiom::Comment<T, uint8_t> { comment_for_glyph(glyph_id) };
        s << Idiom::ArrayLineBreak<T, uint8_t> {};
    }

    s << Idiom::EndArray<T, uint8_t> {};


    auto bytes_per_line = size.width / byte_size + (size.width % byte_size ? 1 : 0);
    auto bytes_per_glyph = size.height * bytes_per_line;

    auto max_offset = (face.exported_glyph_ids().size() - 1) * bytes_per_glyph;

    if (max_offset < (1<<8)) {
        s << subset_lut<T,uint8_t>(face.exported_glyph_ids(), bytes_per_glyph);
    } else if (max_offset < (1<<16)) {
        s << subset_lut<T,uint16_t>(face.exported_glyph_ids(), bytes_per_glyph);
    } else if (max_offset < (1ull<<32)) {
        s << subset_lut<T,uint32_t>(face.exported_glyph_ids(), bytes_per_glyph);
    } else {
        s << subset_lut<T,uint64_t>(face.exported_glyph_ids(), bytes_per_glyph);
    }

    s << Idiom::End<T> {};

    return s.str();
}

template<typename T>
std::string FontSourceCodeGenerator::generate(const Font::Face &face, std::string font_name)
{
    switch (options_.export_method) {
    case SourceCodeOptions::ExportAll:
        return generate_all<T>(face, font_name);
    case SourceCodeOptions::ExportSelected:
        return generate_subset<T>(face, font_name);
    }
}



#endif // FONTSOURCECODEGENERATOR_H
