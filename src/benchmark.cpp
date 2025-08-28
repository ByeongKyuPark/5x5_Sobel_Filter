#include "sobel_filter_simd.hpp"
#include "image_io.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>

/**
 * @brief Performance benchmarking tool for Sobel filter implementations
 * 
 * Demonstrates the performance improvements from SIMD optimization
 * for on-device C++ AI engineer evaluation.
 */
class SobelBenchmark {
public:
    struct BenchmarkResult {
        std::string implementation;
        double avgProcessingTimeMs;
        double pixelsPerSecond;
        double memoryBandwidthMBps;
        double speedupVsScalar;
        std::string cpuFeatures;
    };

    /**
     * @brief Run comprehensive benchmark comparing all implementations
     * @param inputFile Test image file (640x640 RGB)
     * @param iterations Number of benchmark iterations
     * @return Vector of benchmark results
     */
    static std::vector<BenchmarkResult> runBenchmark(const std::string& inputFile, 
                                                    int iterations = 10) {
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║               SOBEL FILTER PERFORMANCE BENCHMARK            ║\n";
        std::cout << "║                  On-Device C++ AI Optimization              ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";

        // Load test image
        auto imageResult = ImageIO::loadRGBImage(inputFile);
        if (!imageResult.hasValue()) {
            std::cout << "Error loading test image: " << static_cast<int>(imageResult.getError()) << std::endl;
            return {};
        }
        
        RGBImage testImage = imageResult.getValue();
        std::cout << "Test image: " << testImage.width() << "x" << testImage.height() 
                  << " (" << (testImage.width() * testImage.height() * 3 / 1024) << " KB)\n";
        std::cout << "CPU Features: " << SobelFilterSIMD::getCPUCapabilities() << "\n";
        std::cout << "Benchmark iterations: " << iterations << "\n\n";

        std::vector<BenchmarkResult> results;
        
        // Test different optimization levels
        std::vector<std::pair<SobelFilterSIMD::OptimizationLevel, std::string>> testCases = {
            {SobelFilterSIMD::OptimizationLevel::SCALAR, "Scalar (Baseline)"},
            {SobelFilterSIMD::OptimizationLevel::SSE, "SSE 4.1 Optimized"},
            {SobelFilterSIMD::OptimizationLevel::AVX2, "AVX2 Optimized"},
        };

        double scalarTime = 0.0;
        
        for (const auto& testCase : testCases) {
            auto result = benchmarkImplementation(testImage, testCase.first, 
                                                testCase.second, iterations);
            if (testCase.first == SobelFilterSIMD::OptimizationLevel::SCALAR) {
                scalarTime = result.avgProcessingTimeMs;
                result.speedupVsScalar = 1.0;
            } else {
                result.speedupVsScalar = scalarTime / result.avgProcessingTimeMs;
            }
            
            results.push_back(result);
        }

        // Print summary table
        printBenchmarkTable(results);
        
        return results;
    }

private:
    static BenchmarkResult benchmarkImplementation(const RGBImage& testImage,
                                                  SobelFilterSIMD::OptimizationLevel level,
                                                  const std::string& name,
                                                  int iterations) {
        std::cout << "Testing " << name << "..." << std::flush;
        
        SobelFilterSIMD filter(level);
        GrayscaleImage output(640, 640);
        std::vector<double> times;
        times.reserve(iterations);
        
        // Warm-up run
        bool warmupResult = filter.apply(testImage, output, false);
        if (!warmupResult) {
            std::cout << " FAILED\n";
            return {};
        }
        
        // Benchmark runs
        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            
            bool result = filter.apply(testImage, output, true);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            times.push_back(duration.count() / 1000.0); // Convert to milliseconds
            
            if (!result) {
                std::cout << " FAILED on iteration " << i << "\n";
                return {};
            }
        }
        
        // Calculate statistics
        double avgTime = 0.0;
        for (double time : times) {
            avgTime += time;
        }
        avgTime /= times.size();
        
        size_t totalPixels = testImage.width() * testImage.height();
        double pixelsPerSec = (totalPixels * 1000.0) / avgTime;
        double memoryBandwidthMBps = (totalPixels * 4 * 1000.0) / (avgTime * 1024 * 1024);
        
        std::cout << " ✓ " << std::fixed << std::setprecision(2) << avgTime << "ms\n";
        
        return {
            name,
            avgTime,
            pixelsPerSec,
            memoryBandwidthMBps,
            0.0, // Will be calculated later
            SobelFilterSIMD::getCPUCapabilities()
        };
    }
    
    static void printBenchmarkTable(const std::vector<BenchmarkResult>& results) {
        std::cout << "\n╔══════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                           BENCHMARK RESULTS                                 ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ Implementation      │ Time (ms) │ Pixels/sec │ Bandwidth │ Speedup │ Notes ║\n";
        std::cout << "╠═════════════════════┼═══════════┼════════════┼═══════════┼═════════┼═══════╣\n";
        
        for (const auto& result : results) {
            std::cout << "║ " << std::left << std::setw(19) << result.implementation << " │ ";
            std::cout << std::right << std::setw(9) << std::fixed << std::setprecision(2) 
                      << result.avgProcessingTimeMs << " │ ";
            std::cout << std::setw(10) << std::scientific << std::setprecision(1) 
                      << result.pixelsPerSecond << " │ ";
            std::cout << std::setw(9) << std::fixed << std::setprecision(1) 
                      << result.memoryBandwidthMBps << " │ ";
            std::cout << std::setw(7) << std::setprecision(2) << result.speedupVsScalar << "x │ ";
            
            std::string notes = "";
            if (result.implementation.find("SSE") != std::string::npos) {
                notes = "SIMD";
            } else if (result.implementation.find("AVX2") != std::string::npos) {
                notes = "SIMD+";
            } else {
                notes = "Base";
            }
            std::cout << std::left << std::setw(5) << notes << " ║\n";
        }
        
        std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝\n";
        
        // Performance analysis
        if (results.size() >= 2) {
            auto bestResult = *std::max_element(results.begin(), results.end(),
                [](const BenchmarkResult& a, const BenchmarkResult& b) {
                    return a.speedupVsScalar < b.speedupVsScalar;
                });
            
            std::cout << "\n🚀 Performance Analysis:\n";
            std::cout << "   • Best implementation: " << bestResult.implementation << "\n";
            std::cout << "   • Maximum speedup: " << std::fixed << std::setprecision(2) 
                      << bestResult.speedupVsScalar << "x over scalar baseline\n";
            std::cout << "   • Peak throughput: " << std::scientific << std::setprecision(2)
                      << bestResult.pixelsPerSecond << " pixels/second\n";
            std::cout << "   • Memory bandwidth: " << std::fixed << std::setprecision(1)
                      << bestResult.memoryBandwidthMBps << " MB/s\n\n";
        }
        
        std::cout << "💡 Optimization Notes:\n";
        std::cout << "   • SIMD instructions provide parallel pixel processing\n";
        std::cout << "   • Memory alignment reduces cache misses\n";
        std::cout << "   • Integer arithmetic avoids floating-point overhead\n";
        std::cout << "   • Suitable for real-time on-device AI applications\n\n";
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <640x640_rgb_image.raw>\n";
        std::cout << "\nThis tool benchmarks SIMD-optimized Sobel filter implementations\n";
        std::cout << "designed for on-device C++ AI applications.\n\n";
        std::cout << "Features demonstrated:\n";
        std::cout << "  • SSE/AVX SIMD vectorization\n";
        std::cout << "  • Memory-efficient algorithms\n";
        std::cout << "  • Performance profiling\n";
        std::cout << "  • Real-time processing capabilities\n";
        return 1;
    }

    try {
        auto results = SobelBenchmark::runBenchmark(argv[1], 10);
        
        if (results.empty()) {
            std::cout << "Benchmark failed to run.\n";
            return 1;
        }
        
        std::cout << "Benchmark completed successfully!\n";
        std::cout << "Results demonstrate C++ optimization techniques for on-device AI.\n";
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cout << "Benchmark error: " << e.what() << std::endl;
        return 1;
    }
}
