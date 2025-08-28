#include "sobel_filter.hpp"
#include "sobel_filter_simd.hpp"
#include "image.hpp"
#include <iostream>

using namespace sobel;

int main() {
    // Create a simple 5x5 test image
    RGBImage testImage(5, 5);
    
    // Fill with a simple pattern
    for (size_t y = 0; y < 5; ++y) {
        for (size_t x = 0; x < 5; ++x) {
            uint8_t value = static_cast<uint8_t>(x * 50);
            testImage.at(x, y) = RGBPixel(value, value, value);
        }
    }
    
    std::cout << "Input 5x5 image:" << std::endl;
    for (size_t y = 0; y < 5; ++y) {
        for (size_t x = 0; x < 5; ++x) {
            auto p = testImage.at(x, y);
            std::cout << (int)p.r << " ";
        }
        std::cout << std::endl;
    }
    
    // Test baseline
    SobelFilter baseline;
    GrayscaleImage baselineResult = baseline.apply(testImage);
    
    // Test SIMD
    SobelFilterSIMD simd;
    GrayscaleImage simdResult;
    simd.apply(testImage, simdResult, false);
    
    std::cout << "\nBaseline result:" << std::endl;
    for (size_t y = 0; y < 5; ++y) {
        for (size_t x = 0; x < 5; ++x) {
            std::cout << (int)baselineResult.at(x, y) << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nSIMD result:" << std::endl;
    for (size_t y = 0; y < 5; ++y) {
        for (size_t x = 0; x < 5; ++x) {
            std::cout << (int)simdResult.at(x, y) << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nDifferences:" << std::endl;
    for (size_t y = 0; y < 5; ++y) {
        for (size_t x = 0; x < 5; ++x) {
            int diff = (int)baselineResult.at(x, y) - (int)simdResult.at(x, y);
            std::cout << diff << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
