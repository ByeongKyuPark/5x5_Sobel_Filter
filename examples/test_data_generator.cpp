/**
 * @file test_data_generator.cpp
 * @brief Generate test RGB data for Sobel filter testing
 * @author BK Park
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdint>

void generateTestImage(const std::string& filename) {
    const int width = 640;
    const int height = 640;
    std::vector<uint8_t> data(width * height * 3);
    
    // Generate a simple test pattern with edges
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 3;
            
            // Create vertical and horizontal stripes for edge detection testing
            uint8_t value = 0;
            
            // Vertical stripes every 80 pixels
            if ((x / 80) % 2 == 0) {
                value = 255;
            }
            
            // Horizontal stripes every 80 pixels
            if ((y / 80) % 2 == 0) {
                value = std::max(value, static_cast<uint8_t>(128));
            }
            
            // Add some diagonal patterns
            if ((x + y) % 160 < 80) {
                value = std::max(value, static_cast<uint8_t>(64));
            }
            
            data[idx] = value;     // R
            data[idx + 1] = value; // G
            data[idx + 2] = value; // B
        }
    }
    
    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    
    std::cout << "Generated test image: " << filename << std::endl;
    std::cout << "Size: " << data.size() << " bytes" << std::endl;
}

int main() {
    std::cout << "Test Data Generator for Sobel Filter\n";
    std::cout << "====================================\n";
    
    generateTestImage("test_input_640x640.raw");
    
    std::cout << "\nTest image ready for processing:\n";
    std::cout << "Usage: sobel_filter.exe test_input_640x640.raw test_output.raw\n";
    
    return 0;
}
