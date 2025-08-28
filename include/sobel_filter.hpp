/**
 * @file sobel_filter.hpp
 * @brief 5x5 Sobel edge detection filter implementation
 * @author BK Park
 * @version 1.0.0
 * @date 2025-08-27
 */

#pragma once

#include "image.hpp"
#include <array>
#include <cstdint>

namespace sobel {

/**
 * @brief 5x5 Sobel kernel type
 */
using SobelKernel5x5 = std::array<std::array<int, 5>, 5>;

/**
 * @brief Configuration for Sobel filter processing
 */
struct SobelConfig {
    bool use_quantization = true;     // Enable quantization
    uint8_t quantization_levels = 64; // Good contrast for edge visualization
    bool normalize_output = true;
    
    SobelConfig() = default;
    
    SobelConfig(bool quantize, uint8_t levels, bool normalize)
        : use_quantization(quantize), quantization_levels(levels), normalize_output(normalize) {}
};

/**
 * @brief 5x5 Sobel edge detection filter
 * 
 * This class implements Sobel edge detection using 5x5 kernels for more accurate
 * edge detection compared to traditional 3x3 kernels. The implementation includes
 * gradient magnitude calculation with optional quantization.
 */
class SobelFilter {
public:
    /**
     * @brief Construct Sobel filter with default configuration
     */
    SobelFilter() = default;
    
    /**
     * @brief Construct Sobel filter with custom configuration
     * @param config Filter configuration
     */
    explicit SobelFilter(const SobelConfig& config);
    
    /**
     * @brief Apply Sobel edge detection to RGB image
     * @param input RGB input image
     * @return Grayscale edge-detected image
     */
    GrayscaleImage apply(const RGBImage& input) const;
    
    /**
     * @brief Apply Sobel edge detection to grayscale image
     * @param input Grayscale input image
     * @return Grayscale edge-detected image
     */
    GrayscaleImage apply(const GrayscaleImage& input) const;
    
    /**
     * @brief Get X-direction Sobel kernel (5x5)
     * @return 5x5 Sobel X kernel
     */
    static const SobelKernel5x5& getKernelX();
    
    /**
     * @brief Get Y-direction Sobel kernel (5x5)
     * @return 5x5 Sobel Y kernel
     */
    static const SobelKernel5x5& getKernelY();
    
    /**
     * @brief Set filter configuration
     * @param config New configuration
     */
    void setConfig(const SobelConfig& config);
    
    /**
     * @brief Get current filter configuration
     * @return Current configuration
     */
    const SobelConfig& getConfig() const noexcept { return config_; }

private:
    SobelConfig config_;
    
    /**
     * @brief Apply convolution with 5x5 kernel
     * @param image Input grayscale image
     * @param kernel 5x5 convolution kernel
     * @return Convolution result as signed values
     */
    std::vector<int16_t> convolve(const GrayscaleImage& image, 
                                  const SobelKernel5x5& kernel) const;
    
    /**
     * @brief Calculate gradient magnitude from X and Y gradients
     * @param gx X-direction gradients
     * @param gy Y-direction gradients
     * @param width Image width
     * @param height Image height
     * @return Gradient magnitudes
     */
    std::vector<double> calculateMagnitude(const std::vector<int16_t>& gx,
                                          const std::vector<int16_t>& gy,
                                          std::size_t width,
                                          std::size_t height) const;
    
    /**
     * @brief Apply quantization to gradient magnitudes
     * @param magnitudes Input gradient magnitudes
     * @return Quantized 8-bit values
     */
    std::vector<uint8_t> quantize(const std::vector<double>& magnitudes) const;
};

} // namespace sobel
