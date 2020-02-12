#include "fontsourcecodegenerator.h"
#include <iomanip>

std::string current_timestamp()
{
    time_t     now = time(nullptr);
    struct tm  tstruct;
    char       buf[23];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d/%m/%Y at %H:%M:%S", &tstruct);

    return buf;
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

