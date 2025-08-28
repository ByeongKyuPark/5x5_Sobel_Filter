#pragma once

#include "image.hpp"
#include "sobel_filter.hpp"
#include <chrono>
#include <string>
#include <memory>
#include <cstddef>

/**
 * @brief Simple SIMD-optimized Sobel filter for demonstration
 */
class SobelFilterSIMD {
public:
    enum class OptimizationLevel {
        SCALAR, SSE, AVX2, AUTO
    };

    struct PerformanceMetrics {
        std::chrono::microseconds processingTime{0};
        size_t pixelsPerSecond = 0;
        size_t memoryBandwidth = 0;
        std::string optimizationUsed;
    };

    explicit SobelFilterSIMD(OptimizationLevel level = OptimizationLevel::AUTO);
    explicit SobelFilterSIMD(const sobel::SobelConfig& config, OptimizationLevel level = OptimizationLevel::AUTO);

    bool apply(const sobel::RGBImage& input, sobel::GrayscaleImage& output, bool enableProfiling = false);

    const PerformanceMetrics& getLastMetrics() const { return lastMetrics_; }
    std::string getCPUCapabilities();
    void setConfig(const sobel::SobelConfig& config) { config_ = config; }

private:
    // --- Configuration / state ---
    sobel::SobelConfig config_;
    OptimizationLevel optimizationLevel_;
    PerformanceMetrics lastMetrics_;
    std::chrono::high_resolution_clock::time_point profilingStart_;

    // --- Step 1 infrastructure (aligned grayscale buffer) ---
    std::unique_ptr<uint8_t[], void(*)(void*)> grayBuffer_{nullptr, &SobelFilterSIMD::alignedDeleter};
    size_t bufferWidth_ = 0;      // original width
    size_t bufferHeight_ = 0;     // original height
    size_t paddedWidth_ = 0;      // width padded to 32-byte alignment (in pixels)

    static void alignedDeleter(void* p);
    static void* alignedAlloc(size_t bytes, size_t alignment);

    void ensureBuffers(size_t width, size_t height);
    void convertRGBToGrayscaleScalar(const sobel::RGBImage& input);

    // Future (Step 2+): SIMD conversion & convolution
    void convertRGBToGrayscaleSSE(const sobel::RGBImage& input);
    void convertRGBToGrayscaleAVX2(const sobel::RGBImage& input);
    void sobel5x5Scalar(const uint8_t* gray, sobel::GrayscaleImage& out);
    void sobel5x5SSE(const uint8_t* gray, sobel::GrayscaleImage& out);
    void sobel5x5AVX2(const uint8_t* gray, sobel::GrayscaleImage& out);

    // Helper for quantization (same logic as baseline)
    std::vector<uint8_t> quantizeWithConfig(const std::vector<double>& magnitudes) const;

    // Profiling helpers
    void startProfiling();
    void endProfiling();
};
