#include "fontsourcecodegenerator.h"
#include <iomanip>

std::string current_timestamp()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream s;
    s << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
    return s.str();
}

std::string comment_for_glyph(std::size_t index)
{
    index += 32;
    std::ostringstream s;
    s << "Character 0x"
      << std::hex << std::setfill('0') << std::setw(2) << index
      << std::dec << " (" << index << ": '"
      << static_cast<char>(index) << "')";

    return s.str();
}

Font::Margins pixel_margins(Font::Margins line_margins, Font::Size glyph_size)
{
    return { line_margins.top * glyph_size.width, line_margins.bottom * glyph_size.width };
}
