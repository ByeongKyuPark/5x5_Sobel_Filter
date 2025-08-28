/**
 * BK Park
 * Sobel Filter Implementation
 * Edge detection using 5x5 Sobel operator for 640x640 RGB images
 */

#include "image.hpp"
#include "image_io.hpp"
#include <iostream>
#include <string>

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " <input.raw> <output.raw>\n";
    std::cout << "  input.raw  : 640x640 RGB raw image file (1,228,800 bytes)\n";
    std::cout << "  output.raw : Output grayscale edge-detected image (409,600 bytes)\n";
    std::cout << "\nImplementation features:\n";
    std::cout << "  - 5x5 Sobel kernels for robust edge detection\n";
    std::cout << "  - RGB to grayscale conversion with proper weighting\n";
    std::cout << "  - Gradient magnitude calculation with quantization\n";
    std::cout << "  - Zero-padding for boundary handling\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Sobel Filter - Edge Detection Implementation\n";
    std::cout << "============================================\n";
    
    if (argc != 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    const std::string inputFile = argv[1];
    const std::string outputFile = argv[2];
    
    std::cout << "Input file: " << inputFile << "\n";
    std::cout << "Output file: " << outputFile << "\n";
    
    // Load RGB image
    constexpr std::size_t IMAGE_WIDTH = 640;
    constexpr std::size_t IMAGE_HEIGHT = 640;
    
    auto result = sobel::ImageIO::loadRGBImage(inputFile, IMAGE_WIDTH, IMAGE_HEIGHT);
    if (!result) {
        std::cerr << "Error loading image: " << toString(result.getError()) << std::endl;
        return 1;
    }
    
    std::cout << "Image loaded successfully (" << result.getValue().width() << "x" 
              << result.getValue().height() << " pixels)\n";
    
    // Convert to grayscale for demonstration
    sobel::GrayscaleImage grayImage(IMAGE_WIDTH, IMAGE_HEIGHT);
    for (std::size_t y = 0; y < IMAGE_HEIGHT; ++y) {
        for (std::size_t x = 0; x < IMAGE_WIDTH; ++x) {
            grayImage.setPixel(x, y, result.getValue().at(x, y).toGrayscale());
        }
    }
    
    std::cout << "Converted to grayscale. Saving output...\n";
    
    // Save grayscale image (placeholder for edge detection)
    auto saveResult = sobel::ImageIO::saveGrayscaleImage(grayImage, outputFile);
    if (!saveResult) {
        std::cerr << "Error saving image: " << toString(saveResult.getError()) << std::endl;
        return 1;
    }
    
    std::cout << "Processing complete! Output saved to: " << outputFile << "\n";
    std::cout << "Note: Sobel edge detection will be implemented in next phase.\n";
    
    return 0;
}
