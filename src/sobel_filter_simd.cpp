#include "sobel_filter_simd.hpp"
#include <immintrin.h>
#include <intrin.h>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <cmath>

SobelFilterSIMD::SobelFilterSIMD(OptimizationLevel level) : config_(), optimizationLevel_(level) {
    if (optimizationLevel_ == OptimizationLevel::AUTO) {
        std::string caps = getCPUCapabilities();
        if (caps.find("AVX2") != std::string::npos) optimizationLevel_ = OptimizationLevel::AVX2;
        else if (caps.find("SSE4.1") != std::string::npos) optimizationLevel_ = OptimizationLevel::SSE;
        else optimizationLevel_ = OptimizationLevel::SCALAR;
    }
}

SobelFilterSIMD::SobelFilterSIMD(const sobel::SobelConfig& config, OptimizationLevel level) 
    : config_(config), optimizationLevel_(level) {
    if (optimizationLevel_ == OptimizationLevel::AUTO) {
        std::string caps = getCPUCapabilities();
        if (caps.find("AVX2") != std::string::npos) optimizationLevel_ = OptimizationLevel::AVX2;
        else if (caps.find("SSE4.1") != std::string::npos) optimizationLevel_ = OptimizationLevel::SSE;
        else optimizationLevel_ = OptimizationLevel::SCALAR;
    }
}

// Aligned allocation helpers
void* SobelFilterSIMD::alignedAlloc(size_t bytes, size_t alignment) {
#if defined(_MSC_VER)
    return _aligned_malloc(bytes, alignment);
#else
    void* p = nullptr;
    if (posix_memalign(&p, alignment, bytes) != 0) return nullptr;
    return p;
#endif
}

void SobelFilterSIMD::alignedDeleter(void* p) {
    if (!p) return;
#if defined(_MSC_VER)
    _aligned_free(p);
#else
    free(p);
#endif
}

void SobelFilterSIMD::ensureBuffers(size_t width, size_t height) {
    if (width == bufferWidth_ && height == bufferHeight_ && grayBuffer_) return;
    bufferWidth_ = width;
    bufferHeight_ = height;
    // Pad width to next multiple of 32 for AVX2 convenience
    paddedWidth_ = (width + 31) & ~size_t(31);
    size_t bytes = paddedWidth_ * height; // 1 byte per grayscale pixel
    grayBuffer_.reset(static_cast<uint8_t*>(alignedAlloc(bytes, 32)));
    if (!grayBuffer_) throw std::bad_alloc();
    std::memset(grayBuffer_.get(), 0, bytes);
}

void SobelFilterSIMD::convertRGBToGrayscaleScalar(const sobel::RGBImage& input) {
    const size_t w = bufferWidth_;
    const size_t h = bufferHeight_;
    uint8_t* dst = grayBuffer_.get();
    // Use EXACT same formula as baseline RGBPixel::toGrayscale()
    constexpr double R_WEIGHT = 0.2126;
    constexpr double G_WEIGHT = 0.7152;
    constexpr double B_WEIGHT = 0.0722;
    
    for (size_t y = 0; y < h; ++y) {
        const auto rowOffsetDst = y * paddedWidth_;
        for (size_t x = 0; x < w; ++x) {
            const auto& p = input.at(x, y);
            double gray = R_WEIGHT * p.r + G_WEIGHT * p.g + B_WEIGHT * p.b;
            dst[rowOffsetDst + x] = static_cast<uint8_t>(std::round(std::clamp(gray, 0.0, 255.0)));
        }
        // Remaining padded bytes already zeroed
    }
}

// SSE RGB->gray - Use scalar for exact baseline compatibility
void SobelFilterSIMD::convertRGBToGrayscaleSSE(const sobel::RGBImage& input) {
    // For perfect accuracy, use same method as baseline
    convertRGBToGrayscaleScalar(input);
}

// AVX2 RGB->gray - Use scalar for exact baseline compatibility  
void SobelFilterSIMD::convertRGBToGrayscaleAVX2(const sobel::RGBImage& input) {
    // For perfect accuracy, use same method as baseline
    convertRGBToGrayscaleScalar(input);
}

// Scalar 5x5 Sobel using grayscale buffer WITH PROPER MAGNITUDE + QUANTIZATION
void SobelFilterSIMD::sobel5x5Scalar(const uint8_t* gray, sobel::GrayscaleImage& out) {
    const int w = (int)bufferWidth_; const int h = (int)bufferHeight_;
    out.resize(bufferWidth_, bufferHeight_);
    
    // 5x5 Sobel kernels
    static const int kx[5][5] = { { -1,-2,0,2,1 }, { -4,-8,0,8,4 }, { -6,-12,0,12,6 }, { -4,-8,0,8,4 }, { -1,-2,0,2,1 } };
    static const int ky[5][5] = { { -1,-4,-6,-4,-1 }, { -2,-8,-12,-8,-2 }, { 0,0,0,0,0 }, { 2,8,12,8,2 }, { 1,4,6,4,1 } };
    
    // Calculate gradients for ALL pixels (like baseline) - use zero padding for boundaries
    std::vector<int16_t> gx(w * h, 0), gy(w * h, 0);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int gx_sum = 0, gy_sum = 0;
            
            // Apply 5x5 kernel with zero-padding (like baseline)
            for (int j = -2; j <= 2; ++j) {
                for (int i = -2; i <= 2; ++i) {
                    int src_y = y + j;
                    int src_x = x + i;
                    
                    // Zero padding for out-of-bounds
                    uint8_t pixel_value = 0;
                    if (src_y >= 0 && src_y < h && src_x >= 0 && src_x < w) {
                        pixel_value = gray[src_y * paddedWidth_ + src_x];
                    }
                    
                    gx_sum += pixel_value * kx[j + 2][i + 2];
                    gy_sum += pixel_value * ky[j + 2][i + 2];
                }
            }
            
            size_t idx = y * w + x;
            gx[idx] = (int16_t)std::clamp(gx_sum, -32768, 32767);
            gy[idx] = (int16_t)std::clamp(gy_sum, -32768, 32767);
        }
    }
    
    // Calculate magnitude using same method as baseline
    std::vector<double> magnitudes(w * h);
    for (size_t i = 0; i < gx.size(); ++i) {
        double magnitude = std::sqrt(
            static_cast<double>(gx[i]) * gx[i] + 
            static_cast<double>(gy[i]) * gy[i]
        );
        magnitudes[i] = magnitude;
    }
    
    // Apply quantization using baseline method
    std::vector<uint8_t> quantized = quantizeWithConfig(magnitudes);
    
    // Copy to output
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            out.at(x, y) = quantized[y * w + x];
        }
    }
}

// Placeholder SIMD 5x5 (will implement later). Currently calls scalar.
void SobelFilterSIMD::sobel5x5SSE(const uint8_t* gray, sobel::GrayscaleImage& out) { sobel5x5Scalar(gray,out); }
void SobelFilterSIMD::sobel5x5AVX2(const uint8_t* gray, sobel::GrayscaleImage& out) { sobel5x5Scalar(gray,out); }

// Quantization helper - same logic as baseline SobelFilter::quantize
std::vector<uint8_t> SobelFilterSIMD::quantizeWithConfig(const std::vector<double>& magnitudes) const {
    if (magnitudes.empty()) return {};
    std::vector<uint8_t> result(magnitudes.size());
    
    if (!config_.use_quantization) {
        for (size_t i = 0; i < magnitudes.size(); ++i) {
            result[i] = static_cast<uint8_t>(std::clamp(magnitudes[i], 0.0, 255.0));
        }
        return result;
    }
    
    auto minmax = std::minmax_element(magnitudes.begin(), magnitudes.end());
    double min_mag = *minmax.first, max_mag = *minmax.second, range = max_mag - min_mag;
    if (range < 1e-10) { std::fill(result.begin(), result.end(), uint8_t(0)); return result; }
    
    double scale = static_cast<double>(config_.quantization_levels) / range;
    for (size_t i = 0; i < magnitudes.size(); ++i) {
        double normalized = (magnitudes[i] - min_mag) * scale;
        if (config_.normalize_output) normalized = (normalized / config_.quantization_levels) * 255.0;
        result[i] = static_cast<uint8_t>(std::clamp(normalized, 0.0, 255.0));
    }
    return result;
}

bool SobelFilterSIMD::apply(const sobel::RGBImage& input, sobel::GrayscaleImage& output, bool enableProfiling) {
    if (enableProfiling) {
        startProfiling();
    }

    ensureBuffers(input.width(), input.height());

    // RGB -> grayscale (choose path)
    switch (optimizationLevel_) {
        case OptimizationLevel::AVX2: convertRGBToGrayscaleAVX2(input); break;
        case OptimizationLevel::SSE:  convertRGBToGrayscaleSSE(input);  break;
        default: convertRGBToGrayscaleScalar(input); break;
    }

    // 5x5 Sobel (currently scalar; SIMD later)
    switch (optimizationLevel_) {
        case OptimizationLevel::AVX2: sobel5x5AVX2(grayBuffer_.get(), output); break;
        case OptimizationLevel::SSE:  sobel5x5SSE(grayBuffer_.get(), output);  break;
        default: sobel5x5Scalar(grayBuffer_.get(), output); break;
    }

    if (enableProfiling) {
        endProfiling();
        
        // Calculate basic performance metrics
        auto timeUs = lastMetrics_.processingTime.count();
        size_t totalPixels = input.width() * input.height();
        lastMetrics_.pixelsPerSecond = (timeUs > 0) ? (totalPixels * 1000000ull) / timeUs : 0;
        lastMetrics_.memoryBandwidth = totalPixels; // bytes read approx
        
        switch (optimizationLevel_) {
            case OptimizationLevel::AVX2:
                lastMetrics_.optimizationUsed = "AVX2";
                break;
            case OptimizationLevel::SSE:
                lastMetrics_.optimizationUsed = "SSE";
                break;
            default:
                lastMetrics_.optimizationUsed = "Scalar";
                break;
        }
    }

    return true;
}

std::string SobelFilterSIMD::getCPUCapabilities() {
    std::stringstream ss;
    
#ifdef _MSC_VER
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 1);
    
    bool sse = (cpuInfo[3] & (1 << 25)) != 0;
    bool sse2 = (cpuInfo[3] & (1 << 26)) != 0;
    bool sse3 = (cpuInfo[2] & (1 << 0)) != 0;
    bool sse41 = (cpuInfo[2] & (1 << 19)) != 0;
    bool sse42 = (cpuInfo[2] & (1 << 20)) != 0;
    bool avx = (cpuInfo[2] & (1 << 28)) != 0;
    
    if (sse) ss << "SSE ";
    if (sse2) ss << "SSE2 ";
    if (sse3) ss << "SSE3 ";
    if (sse41) ss << "SSE4.1 ";
    if (sse42) ss << "SSE4.2 ";
    if (avx) ss << "AVX ";
    
    // Check for AVX2
    if (avx) {
        __cpuidex(cpuInfo, 7, 0);
        bool avx2 = (cpuInfo[1] & (1 << 5)) != 0;
        if (avx2) ss << "AVX2 ";
    }
#else
    ss << "Unknown ";
#endif
    
    return ss.str();
}

void SobelFilterSIMD::startProfiling() {
    profilingStart_ = std::chrono::high_resolution_clock::now();
}

void SobelFilterSIMD::endProfiling() {
    auto end = std::chrono::high_resolution_clock::now();
    lastMetrics_.processingTime = std::chrono::duration_cast<std::chrono::microseconds>(
        end - profilingStart_);
}
