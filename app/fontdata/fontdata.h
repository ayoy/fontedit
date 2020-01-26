#ifndef FONTDATA_H
#define FONTDATA_H

#include <vector>

class Glyph_data {

public:
    explicit Glyph_data(size_t width, size_t height);
    explicit Glyph_data(size_t width, size_t height, std::vector<bool> pixels);

    size_t width() const { return width_; }
    size_t height() const { return height_; }

private:
    size_t width_;
    size_t height_;
    std::vector<bool> pixels_;
};


class Font_data
{
public:
    Font_data() = default;
};

#endif // FONTDATA_H
