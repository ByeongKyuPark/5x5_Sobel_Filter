#include "../include/sobel_filter.hpp"
#include "../include/image_io.hpp"
#include <iostream>

int main() {
    // Load test image
    auto result = sobel::ImageIO::loadRGBImage("building_test.raw", 640, 640);
    if (!result) {
        std::cerr << "Failed to load test image" << std::endl;
        return 1;
    }
    
    std::cout << "=== SOBEL DEBUG ===" << std::endl;
    
    // Convert to grayscale
    sobel::GrayscaleImage gray(640, 640);
    for (size_t y = 0; y < 640; ++y) {
        for (size_t x = 0; x < 640; ++x) {
            gray.setPixel(x, y, result.getValue().at(x, y).toGrayscale());
        }
    }
    
    // Apply Sobel kernels manually
    sobel::SobelFilter filter;
    
    // Get the raw gradients before quantization
    std::cout << "Sample pixel values:" << std::endl;
    for (int i = 0; i < 10; i++) {
        uint8_t pixel = gray.at(i, 0);
        std::cout << "Pixel[" << i << ",0] = " << (int)pixel << std::endl;
    }
    
    return 0;
}
