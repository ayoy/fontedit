#ifndef INPUTIMAGE_H
#define INPUTIMAGE_H

#include <cstdint>

class InputImage
{
public:

    /**
     * @brief width
     * @return The width of the image in pixels.
     */
    virtual uint32_t width() const = 0;

    /**
     * @brief height
     * @return The height of the image in pixels.
     */
    virtual uint32_t height() const = 0;

    /**
     * @brief isPixelSet
     * @param x Horizontal axis coordinate of the pixel
     * @param y Vertical axis coordinate of the pixel
     * @return true if a given pixel is set in the image, false otherwise
     */
    virtual bool isPixelSet(uint32_t x, uint32_t y) const = 0;

    virtual ~InputImage() = default;
};

#endif // INPUTIMAGE_H
