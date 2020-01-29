#include "fontdata.h"
#include "bmp.h"

Glyph::Glyph(size_t width, size_t height) :
    width_ { width },
    height_ { height },
    pixels_ { std::vector<bool>(width * height, false) }
{}

Glyph::Glyph(size_t width, size_t height, std::vector<bool> pixels) :
    width_ { width },
    height_ { height },
    pixels_ { std::move(pixels) }
{}

FontFace::FontFace(std::istringstream &bitmap_data)
{
    std::istream bitmap_stream { bitmap_data.rdbuf() };
    BMP p(bitmap_stream);

    if (p.bmp_info_header.bit_count != 1) {
        throw std::runtime_error("Error! 1-bit BMP is required");
    }

    std::cout << "width: " << p.bmp_info_header.width << "px" << std::endl
              << "height: " << p.bmp_info_header.height << "px" << std::endl
              << "bit count: " << p.bmp_info_header.bit_count << std::endl;

    for (const auto &b : p.data) {
        std::cout << "0x"
                  << std::setw(2)
                  << std::setfill('0')
                  << std::hex
                  << static_cast<uint>(b)
                  << ", ";
    }
}
