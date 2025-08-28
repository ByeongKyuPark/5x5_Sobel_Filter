/**
 * @file validation_test.cpp
 * @brief Validation tests for SIMD Sobel filter implementation
 * @author BK Park
 * @date 2025-08-27
 */

#include "sobel_filter.hpp"
#include "sobel_filter_simd.hpp"
#include "image.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

using namespace sobel;

struct TestResult {
    bool passed;
    std::string testName;
    std::string details;
    double maxPixelDiff;
    double meanPixelDiff;
};

class ValidationTest {
private:
    std::vector<TestResult> results_;
    
    // Create test images
    RGBImage createSolidColorImage(size_t width, size_t height, uint8_t r, uint8_t g, uint8_t b) {
        RGBImage img(width, height);
        RGBPixel color(r, g, b);
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                img.at(x, y) = color;
            }
        }
        return img;
    }
    
    RGBImage createGradientImage(size_t width, size_t height) {
        RGBImage img(width, height);
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                uint8_t intensity = static_cast<uint8_t>((x * 255) / width);
                img.at(x, y) = RGBPixel(intensity, intensity, intensity);
            }
        }
        return img;
    }
    
    RGBImage createRandomImage(size_t width, size_t height, uint32_t seed = 42) {
        RGBImage img(width, height);
        std::mt19937 rng(seed);
        std::uniform_int_distribution<int> dist(0, 255);
        
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                img.at(x, y) = RGBPixel(static_cast<uint8_t>(dist(rng)), 
                                       static_cast<uint8_t>(dist(rng)), 
                                       static_cast<uint8_t>(dist(rng)));
            }
        }
        return img;
    }
    
    RGBImage createCheckerboardImage(size_t width, size_t height, size_t checkSize = 8) {
        RGBImage img(width, height);
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                bool isWhite = ((x / checkSize) + (y / checkSize)) % 2 == 0;
                uint8_t value = isWhite ? 255 : 0;
                img.at(x, y) = RGBPixel(value, value, value);
            }
        }
        return img;
    }
    
    // Compare two grayscale images
    TestResult compareImages(const GrayscaleImage& baseline, const GrayscaleImage& simd, 
                           const std::string& testName, double maxAllowedDiff = 1.0) {
        TestResult result;
        result.testName = testName;
        result.passed = false;
        result.maxPixelDiff = 0.0;
        result.meanPixelDiff = 0.0;
        
        if (baseline.width() != simd.width() || baseline.height() != simd.height()) {
            result.details = "Image dimensions mismatch";
            return result;
        }
        
        double sumDiff = 0.0;
        size_t totalPixels = baseline.width() * baseline.height();
        
        for (size_t y = 0; y < baseline.height(); ++y) {
            for (size_t x = 0; x < baseline.width(); ++x) {
                double diff = std::abs(static_cast<double>(baseline.at(x, y)) - 
                                     static_cast<double>(simd.at(x, y)));
                result.maxPixelDiff = std::max(result.maxPixelDiff, diff);
                sumDiff += diff;
            }
        }
        
        result.meanPixelDiff = sumDiff / totalPixels;
        result.passed = result.maxPixelDiff <= maxAllowedDiff;
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) 
            << "Max diff: " << result.maxPixelDiff 
            << ", Mean diff: " << result.meanPixelDiff
            << " (threshold: " << maxAllowedDiff << ")";
        result.details = oss.str();
        
        return result;
    }
    
public:
    void testSIMDCorrectness() {
        std::cout << "\n=== SIMD Correctness Tests ===" << std::endl;
        
        // Test configurations
        std::vector<std::pair<SobelConfig, std::string>> configs = {
            {SobelConfig(true, 255, true), "Default (quant=255, norm=true)"},
            {SobelConfig(true, 128, true), "Quantized (quant=128, norm=true)"},
            {SobelConfig(true, 64, true), "Heavy quantization (quant=64, norm=true)"},
            {SobelConfig(true, 255, false), "No normalization (quant=255, norm=false)"},
            {SobelConfig(false, 255, false), "No quantization (quant=off, norm=false)"}
        };
        
        // Test images
        std::vector<std::pair<RGBImage, std::string>> testImages = {
            {createSolidColorImage(64, 64, 128, 128, 128), "Solid Gray"},
            {createSolidColorImage(64, 64, 0, 0, 0), "Solid Black"},
            {createSolidColorImage(64, 64, 255, 255, 255), "Solid White"},
            {createGradientImage(64, 64), "Horizontal Gradient"},
            {createCheckerboardImage(64, 64, 4), "Checkerboard 4x4"},
            {createRandomImage(64, 64), "Random Noise"}
        };
        
        for (const auto& [config, configName] : configs) {
            for (const auto& [testImage, imageName] : testImages) {
                // Test each optimization level
                std::vector<std::pair<SobelFilterSIMD::OptimizationLevel, std::string>> levels = {
                    {SobelFilterSIMD::OptimizationLevel::SCALAR, "Scalar"},
                    {SobelFilterSIMD::OptimizationLevel::SSE, "SSE"},
                    {SobelFilterSIMD::OptimizationLevel::AVX2, "AVX2"}
                };
                
                // Get baseline result
                SobelFilter baseline(config);
                GrayscaleImage baselineResult = baseline.apply(testImage);
                
                for (const auto& [level, levelName] : levels) {
                    SobelFilterSIMD simdFilter(config, level);
                    GrayscaleImage simdResult;
                    simdFilter.apply(testImage, simdResult, false);
                    
                    std::string testName = configName + " | " + imageName + " | " + levelName;
                    TestResult result = compareImages(baselineResult, simdResult, testName, 1.0);
                    results_.push_back(result);
                    
                    std::cout << (result.passed ? "✅ PASS" : "❌ FAIL") 
                              << " " << testName << std::endl;
                    std::cout << "   " << result.details << std::endl;
                }
            }
        }
    }
    
    void testQuantizationLevels() {
        std::cout << "\n=== Quantization Level Tests ===" << std::endl;
        
        RGBImage testImage = createGradientImage(32, 32);
        
        std::vector<uint8_t> quantLevels = {16, 32, 64, 128, 255};
        
        for (uint8_t levels : quantLevels) {
            SobelConfig config(true, levels, true);
            
            SobelFilter baseline(config);
            GrayscaleImage baselineResult = baseline.apply(testImage);
            
            SobelFilterSIMD simdFilter(config);
            GrayscaleImage simdResult;
            simdFilter.apply(testImage, simdResult, false);
            
            std::string testName = "Quantization levels: " + std::to_string(levels);
            TestResult result = compareImages(baselineResult, simdResult, testName, 1.0);
            results_.push_back(result);
            
            std::cout << (result.passed ? "✅ PASS" : "❌ FAIL") 
                      << " " << testName << std::endl;
            std::cout << "   " << result.details << std::endl;
        }
    }
    
    void testEdgeCases() {
        std::cout << "\n=== Edge Case Tests ===" << std::endl;
        
        std::vector<std::pair<RGBImage, std::string>> edgeCases = {
            {createSolidColorImage(1, 1, 128, 128, 128), "1x1 image"},
            {createSolidColorImage(3, 3, 128, 128, 128), "3x3 image (minimal for 5x5)"},
            {createSolidColorImage(5, 5, 128, 128, 128), "5x5 image (exact kernel size)"},
            {createSolidColorImage(640, 480, 128, 128, 128), "VGA resolution"},
            {createSolidColorImage(33, 17, 255, 0, 128), "Odd dimensions"},
        };
        
        SobelConfig config; // Default config
        
        for (const auto& [testImage, description] : edgeCases) {
            SobelFilter baseline(config);
            SobelFilterSIMD simdFilter(config);
            
            try {
                GrayscaleImage baselineResult = baseline.apply(testImage);
                GrayscaleImage simdResult;
                bool success = simdFilter.apply(testImage, simdResult, false);
                
                std::string testName = "Edge case: " + description;
                
                if (!success) {
                    results_.push_back({false, testName, "SIMD apply failed", 0, 0});
                    std::cout << "❌ FAIL " << testName << " - SIMD apply failed" << std::endl;
                    continue;
                }
                
                TestResult result = compareImages(baselineResult, simdResult, testName, 1.0);
                results_.push_back(result);
                
                std::cout << (result.passed ? "✅ PASS" : "❌ FAIL") 
                          << " " << testName << std::endl;
                std::cout << "   " << result.details << std::endl;
                
            } catch (const std::exception& e) {
                std::string testName = "Edge case: " + description;
                results_.push_back({false, testName, "Exception: " + std::string(e.what()), 0, 0});
                std::cout << "❌ FAIL " << testName << " - Exception: " << e.what() << std::endl;
            }
        }
    }
    
    void printSummary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        
        size_t totalTests = results_.size();
        size_t passedTests = std::count_if(results_.begin(), results_.end(), 
                                         [](const TestResult& r) { return r.passed; });
        
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << " (" 
                  << std::fixed << std::setprecision(1) 
                  << (100.0 * passedTests / totalTests) << "%)" << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        
        if (passedTests == totalTests) {
            std::cout << "\n🎉 ALL TESTS PASSED! SIMD implementation is correct." << std::endl;
        } else {
            std::cout << "\n⚠️  Some tests failed. Check implementation." << std::endl;
            
            std::cout << "\nFailed tests:" << std::endl;
            for (const auto& result : results_) {
                if (!result.passed) {
                    std::cout << "  ❌ " << result.testName << " - " << result.details << std::endl;
                }
            }
        }
    }
    
    void runAllTests() {
        std::cout << "Starting SIMD Sobel Filter Validation Tests..." << std::endl;
        
        testSIMDCorrectness();
        testQuantizationLevels(); 
        testEdgeCases();
        
        printSummary();
    }
};

int main() {
    try {
        ValidationTest validator;
        validator.runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test framework error: " << e.what() << std::endl;
        return 1;
    }
}
