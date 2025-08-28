/**
 * @file image.hpp
 * @brief Image data structure and basic operations for Sobel edge detection
 * @author BK Park
 * @version 1.0.0
 * @date 2025-08-27
 */

#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <memory>

namespace sobel {

/**
 * @brief RGB pixel structure with 8-bit components
 */
struct RGBPixel {
    uint8_t r, g, b;
    
    RGBPixel() : r(0), g(0), b(0) {}
    RGBPixel(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}
    
    /**
     * @brief Convert RGB to grayscale using ITU-R BT.709 standard
     * @return Grayscale value in range [0, 255]
     */
    uint8_t toGrayscale() const;
};

/**
 * @brief Generic image class template for different pixel types
 * @tparam PixelType Type of pixel (RGBPixel, uint8_t, etc.)
 */
template<typename PixelType>
class Image {
public:
    using pixel_type = PixelType;
    using size_type = std::size_t;
    
private:
    std::vector<PixelType> data_;
    size_type width_;
    size_type height_;
    
public:
    /**
     * @brief Construct empty image
     */
    Image() : width_(0), height_(0) {}
    
    /**
     * @brief Construct image with specified dimensions
     * @param width Image width in pixels
     * @param height Image height in pixels
     */
    Image(size_type width, size_type height);
    
    /**
     * @brief Construct image from raw data
     * @param data Raw pixel data
     * @param width Image width
     * @param height Image height
     */
    Image(std::vector<PixelType> data, size_type width, size_type height);
    
    // Accessors
    size_type width() const noexcept { return width_; }
    size_type height() const noexcept { return height_; }
    size_type size() const noexcept { return data_.size(); }
    bool empty() const noexcept { return data_.empty(); }
    
    /**
     * @brief Get pixel at specified coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @return Reference to pixel
     * @throws std::out_of_range if coordinates are invalid
     */
    const PixelType& at(size_type x, size_type y) const;
    PixelType& at(size_type x, size_type y);
    
    /**
     * @brief Get pixel with bounds checking (safe access)
     * @param x X coordinate
     * @param y Y coordinate
     * @param default_value Value to return if out of bounds
     * @return Pixel value or default_value
     */
    PixelType getPixelSafe(int x, int y, const PixelType& default_value = PixelType{}) const;
    
    /**
     * @brief Set pixel at specified coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @param pixel Pixel value to set
     */
    void setPixel(size_type x, size_type y, const PixelType& pixel);
    
    /**
     * @brief Get raw data pointer
     * @return Pointer to raw data
     */
    const PixelType* data() const noexcept { return data_.data(); }
    PixelType* data() noexcept { return data_.data(); }
    
    /**
     * @brief Clear image data
     */
    void clear();
    
    /**
     * @brief Resize image (destroys existing data)
     * @param width New width
     * @param height New height
     */
    void resize(size_type width, size_type height);
};

// Type aliases for common image types
using RGBImage = Image<RGBPixel>;
using GrayscaleImage = Image<uint8_t>;

} // namespace sobel
