#include "sobel_filter_simd.hpp"
#include "sobel_filter.hpp"
#include "image.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace sobel;
#include <thread>

using namespace sobel;

// Function to create a test image with gradient patterns
RGBImage createTestImage() {
    RGBImage image(640, 640);
    
    for (size_t y = 0; y < 640; ++y) {
        for (size_t x = 0; x < 640; ++x) {
            // Create a gradient pattern with some edges
            uint8_t r = static_cast<uint8_t>((x * 255) / 640);
            uint8_t g = static_cast<uint8_t>((y * 255) / 640);
            uint8_t b = static_cast<uint8_t>(((x + y) * 255) / 1280);
            
            // Add some sharp edges every 100 pixels
            if (x % 100 == 0 || y % 100 == 0) {
                r = g = b = 255;
            }
            
            image.setPixel(x, y, RGBPixel(r, g, b));
        }
    }
    
    return image;
}

void runBenchmark() {
    std::cout << "=== 5x5 Sobel Filter SIMD Benchmark ===" << std::endl;
    std::cout << "Image Size: 640x640 RGB" << std::endl;
    std::cout << "Filter: 5x5 Sobel edge detection" << std::endl << std::endl;
    
    // Create test image
    RGBImage testImage = createTestImage();
    
    // Test different optimization levels
    std::vector<SobelFilterSIMD::OptimizationLevel> levels = {
        SobelFilterSIMD::OptimizationLevel::SCALAR,
        SobelFilterSIMD::OptimizationLevel::SSE,
        SobelFilterSIMD::OptimizationLevel::AVX2
    };
    
    std::vector<std::string> levelNames = {"Scalar", "SSE4.1", "AVX2"};
    
    GrayscaleImage output(640, 640);
    
    for (size_t i = 0; i < levels.size(); ++i) {
        SobelFilterSIMD filter(levels[i]);
        
        std::cout << "Testing " << levelNames[i] << " optimization:" << std::endl;
        std::cout << "CPU Capabilities: " << filter.getCPUCapabilities() << std::endl;
        
        // Warm-up run
        filter.apply(testImage, output, false);
        
        // Timed runs
        const int numRuns = 10;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int run = 0; run < numRuns; ++run) {
            filter.apply(testImage, output, false);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        auto avgTime = totalTime / numRuns;
        
        // Run once with profiling for detailed metrics
        filter.apply(testImage, output, true);
        auto metrics = filter.getLastMetrics();
        
        std::cout << "  Average processing time: " << std::fixed << std::setprecision(2) 
                  << avgTime.count() / 1000.0 << " ms" << std::endl;
        std::cout << "  Pixels per second: " << metrics.pixelsPerSecond << std::endl;
        std::cout << "  Memory bandwidth: " << std::fixed << std::setprecision(1)
                  << metrics.memoryBandwidth / (1024.0 * 1024.0) << " MB/s" << std::endl;
        std::cout << "  Optimization used: " << metrics.optimizationUsed << std::endl;
        
        // Calculate edge statistics
        size_t edgePixels = 0;
        size_t totalPixels = 640 * 640;
        uint64_t intensitySum = 0;
        uint8_t minIntensity = 255, maxIntensity = 0;
        
        for (size_t y = 0; y < 640; ++y) {
            for (size_t x = 0; x < 640; ++x) {
                uint8_t pixel = output.at(x, y);
                if (pixel > 30) edgePixels++; // Threshold for edge detection
                intensitySum += pixel;
                minIntensity = std::min(minIntensity, pixel);
                maxIntensity = std::max(maxIntensity, pixel);
            }
        }
        
        double edgePercentage = (100.0 * edgePixels) / totalPixels;
        double meanIntensity = static_cast<double>(intensitySum) / totalPixels;
        
        std::cout << "  Edge pixels (>30): " << std::fixed << std::setprecision(2) 
                  << edgePercentage << "%" << std::endl;
        std::cout << "  Mean intensity: " << std::fixed << std::setprecision(2) 
                  << meanIntensity << std::endl;
        std::cout << "  Intensity range: " << static_cast<int>(minIntensity) 
                  << " - " << static_cast<int>(maxIntensity) << std::endl;
        
        std::cout << std::endl;
    }
    
    // Compare with baseline implementation
    std::cout << "=== Baseline Comparison ===" << std::endl;
    SobelFilter baselineFilter;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    output = baselineFilter.apply(testImage);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto baselineTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "Baseline (Phase 2) processing time: " << std::fixed << std::setprecision(2)
              << baselineTime.count() / 1000.0 << " ms" << std::endl;
    
    // Calculate speedup ratios
    SobelFilterSIMD sseFilter(SobelFilterSIMD::OptimizationLevel::SSE);
    startTime = std::chrono::high_resolution_clock::now();
    sseFilter.apply(testImage, output, false);
    endTime = std::chrono::high_resolution_clock::now();
    auto sseTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    double speedup = static_cast<double>(baselineTime.count()) / sseTime.count();
    std::cout << "SSE speedup: " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
    
    std::cout << std::endl << "=== On-Device AI Performance Characteristics ===" << std::endl;
    std::cout << "- Memory access pattern: Cache-friendly sequential processing" << std::endl;
    std::cout << "- SIMD utilization: " << sseFilter.getCPUCapabilities() << std::endl;
    std::cout << "- Thread scalability: " << std::thread::hardware_concurrency() << " cores available" << std::endl;
    std::cout << "- Real-time capability: " << (sseTime.count() < 16667 ? "YES" : "NO") 
              << " (60 FPS = 16.67ms budget)" << std::endl;
}

int main() {
    try {
        runBenchmark();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
