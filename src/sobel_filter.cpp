/**
 * @file sobel_filter.cpp
 * @brief Implementation of 5x5 Sobel edge detection filter
 * @author BK Park
 * @version 1.0.0
 * @date 2025-08-27
 */

#include "sobel_filter.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace sobel {

// 5x5 Sobel kernels for more accurate edge detection
const SobelKernel5x5 SOBEL_X_5x5 = {{
    {{-1, -2,  0,  2,  1}},
    {{-4, -8,  0,  8,  4}},
    {{-6, -12, 0, 12,  6}},
    {{-4, -8,  0,  8,  4}},
    {{-1, -2,  0,  2,  1}}
}};

const SobelKernel5x5 SOBEL_Y_5x5 = {{
    {{-1, -4, -6, -4, -1}},
    {{-2, -8, -12, -8, -2}},
    {{ 0,  0,  0,  0,  0}},
    {{ 2,  8, 12,  8,  2}},
    {{ 1,  4,  6,  4,  1}}
}};

SobelFilter::SobelFilter(const SobelConfig& config) : config_(config) {}

GrayscaleImage SobelFilter::apply(const RGBImage& input) const {
    // Convert RGB to grayscale first
    GrayscaleImage grayscale(input.width(), input.height());
    
    for (std::size_t y = 0; y < input.height(); ++y) {
        for (std::size_t x = 0; x < input.width(); ++x) {
            grayscale.setPixel(x, y, input.at(x, y).toGrayscale());
        }
    }
    
    return apply(grayscale);
}

GrayscaleImage SobelFilter::apply(const GrayscaleImage& input) const {
    if (input.empty()) {
        return GrayscaleImage();
    }
    
    const std::size_t width = input.width();
    const std::size_t height = input.height();
    
    // Apply convolution with both kernels
    auto gx = convolve(input, SOBEL_X_5x5);
    auto gy = convolve(input, SOBEL_Y_5x5);
    
    // Calculate gradient magnitudes
    auto magnitudes = calculateMagnitude(gx, gy, width, height);
    
    // Apply quantization
    auto quantized = quantize(magnitudes);
    
    // Create output image
    GrayscaleImage result(width, height);
    for (std::size_t i = 0; i < quantized.size(); ++i) {
        std::size_t x = i % width;
        std::size_t y = i / width;
        result.setPixel(x, y, quantized[i]);
    }
    
    return result;
}

const SobelKernel5x5& SobelFilter::getKernelX() {
    return SOBEL_X_5x5;
}

const SobelKernel5x5& SobelFilter::getKernelY() {
    return SOBEL_Y_5x5;
}

void SobelFilter::setConfig(const SobelConfig& config) {
    config_ = config;
}

std::vector<int16_t> SobelFilter::convolve(const GrayscaleImage& image, 
                                          const SobelKernel5x5& kernel) const {
    const std::size_t width = image.width();
    const std::size_t height = image.height();
    const int kernel_size = 5;
    const int kernel_half = kernel_size / 2;
    
    std::vector<int16_t> result(width * height, 0);
    
    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width; ++x) {
            int32_t sum = 0;
            
            // Apply 5x5 kernel with zero-padding
            for (int ky = -kernel_half; ky <= kernel_half; ++ky) {
                for (int kx = -kernel_half; kx <= kernel_half; ++kx) {
                    // Use safe pixel access with zero padding
                    uint8_t pixel_value = image.getPixelSafe(
                        static_cast<int>(x) + kx, 
                        static_cast<int>(y) + ky, 
                        0  // zero padding
                    );
                    
                    int kernel_value = kernel[ky + kernel_half][kx + kernel_half];
                    sum += pixel_value * kernel_value;
                }
            }
            
            // Clamp to int16_t range to prevent overflow
            sum = std::clamp(sum, 
                            static_cast<int32_t>(std::numeric_limits<int16_t>::min()), 
                            static_cast<int32_t>(std::numeric_limits<int16_t>::max()));
            
            result[y * width + x] = static_cast<int16_t>(sum);
        }
    }
    
    return result;
}

std::vector<double> SobelFilter::calculateMagnitude(const std::vector<int16_t>& gx,
                                                   const std::vector<int16_t>& gy,
                                                   std::size_t width,
                                                   std::size_t height) const {
    std::vector<double> magnitudes(width * height);
    
    for (std::size_t i = 0; i < gx.size(); ++i) {
        // Calculate gradient magnitude: sqrt(gx^2 + gy^2)
        double magnitude = std::sqrt(
            static_cast<double>(gx[i]) * gx[i] + 
            static_cast<double>(gy[i]) * gy[i]
        );
        magnitudes[i] = magnitude;
    }
    
    return magnitudes;
}

std::vector<uint8_t> SobelFilter::quantize(const std::vector<double>& magnitudes) const {
    if (magnitudes.empty()) {
        return {};
    }
    
    std::vector<uint8_t> result(magnitudes.size());
    
    if (!config_.use_quantization) {
        // Simple clamping without normalization
        for (std::size_t i = 0; i < magnitudes.size(); ++i) {
            result[i] = static_cast<uint8_t>(
                std::clamp(magnitudes[i], 0.0, 255.0)
            );
        }
        return result;
    }
    
    // Find min and max for normalization
    auto minmax = std::minmax_element(magnitudes.begin(), magnitudes.end());
    double min_mag = *minmax.first;
    double max_mag = *minmax.second;
    
    // Avoid division by zero
    double range = max_mag - min_mag;
    if (range < 1e-10) {
        std::fill(result.begin(), result.end(), static_cast<uint8_t>(0));
        return result;
    }
    
    // Normalize and quantize
    double scale = static_cast<double>(config_.quantization_levels) / range;
    
    for (std::size_t i = 0; i < magnitudes.size(); ++i) {
        double normalized = (magnitudes[i] - min_mag) * scale;
        
        if (config_.normalize_output) {
            // Map to full 0-255 range
            normalized = (normalized / config_.quantization_levels) * 255.0;
        }
        
        result[i] = static_cast<uint8_t>(
            std::clamp(normalized, 0.0, 255.0)
        );
    }
    
    return result;
}

} // namespace sobel
