/**
 * @file image.cpp
 * @brief Implementation of image data structures and operations
 * @author BK Park
 * @version 1.0.0
 * @date 2025-08-27
 */

#include "image.hpp"
#include <stdexcept>
#include <algorithm>
#include <cmath>

namespace sobel {

uint8_t RGBPixel::toGrayscale() const {
    // ITU-R BT.709 standard weights for RGB to grayscale conversion
    constexpr double R_WEIGHT = 0.2126;
    constexpr double G_WEIGHT = 0.7152;
    constexpr double B_WEIGHT = 0.0722;
    
    double gray = R_WEIGHT * r + G_WEIGHT * g + B_WEIGHT * b;
    return static_cast<uint8_t>(std::round(std::clamp(gray, 0.0, 255.0)));
}

// Template specializations for Image class
template<typename PixelType>
Image<PixelType>::Image(size_type width, size_type height)
    : data_(width * height), width_(width), height_(height) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("Image dimensions must be positive");
    }
}

template<typename PixelType>
Image<PixelType>::Image(std::vector<PixelType> data, size_type width, size_type height)
    : data_(std::move(data)), width_(width), height_(height) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("Image dimensions must be positive");
    }
    if (data_.size() != width * height) {
        throw std::invalid_argument("Data size doesn't match image dimensions");
    }
}

template<typename PixelType>
const PixelType& Image<PixelType>::at(size_type x, size_type y) const {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Pixel coordinates out of bounds");
    }
    return data_[y * width_ + x];
}

template<typename PixelType>
PixelType& Image<PixelType>::at(size_type x, size_type y) {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Pixel coordinates out of bounds");
    }
    return data_[y * width_ + x];
}

template<typename PixelType>
PixelType Image<PixelType>::getPixelSafe(int x, int y, const PixelType& default_value) const {
    if (x < 0 || y < 0 || static_cast<size_type>(x) >= width_ || static_cast<size_type>(y) >= height_) {
        return default_value;
    }
    return data_[y * width_ + x];
}

template<typename PixelType>
void Image<PixelType>::setPixel(size_type x, size_type y, const PixelType& pixel) {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Pixel coordinates out of bounds");
    }
    data_[y * width_ + x] = pixel;
}

template<typename PixelType>
void Image<PixelType>::clear() {
    data_.clear();
    width_ = 0;
    height_ = 0;
}

template<typename PixelType>
void Image<PixelType>::resize(size_type width, size_type height) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("Image dimensions must be positive");
    }
    width_ = width;
    height_ = height;
    data_.resize(width * height);
}

// Explicit template instantiations for common types
template class Image<RGBPixel>;
template class Image<uint8_t>;

} // namespace sobel
