#include "fontsourcecodegenerator.h"
#include <iomanip>
#include <string>

namespace f2b {

font::margins pixel_margins(font::margins line_margins, font::glyph_size glyph_size)
{
    return { line_margins.top * glyph_size.width, line_margins.bottom * glyph_size.width };
}

std::string font_source_code_generator::current_timestamp()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream s;
    s << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
    return s.str();
}

std::string font_source_code_generator::comment_for_glyph(std::size_t index)
{
    index += 32;
    std::ostringstream s;
    s << "Character 0x"
      << std::hex << std::setfill('0') << std::setw(2) << index
      << std::dec << " (" << index;

    if (std::isprint(index)) {
        s << ": '" << static_cast<char>(index) << "'";
    }

    s << ")";

    return s.str();
}

}
