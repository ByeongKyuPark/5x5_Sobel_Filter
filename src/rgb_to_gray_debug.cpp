#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input_rgb.raw> <output_gray.raw>" << std::endl;
        return 1;
    }
    
    // Read RGB data
    std::ifstream input(argv[1], std::ios::binary);
    std::vector<unsigned char> rgb_data(640 * 640 * 3);
    input.read(reinterpret_cast<char*>(&rgb_data[0]), 640 * 640 * 3);
    input.close();
    
    // Convert to grayscale using ITU-R BT.709 weights (same as Sobel filter)
    std::vector<unsigned char> gray_data(640 * 640);
    for (int i = 0; i < 640 * 640; i++) {
        unsigned char r = rgb_data[i * 3 + 0];
        unsigned char g = rgb_data[i * 3 + 1];
        unsigned char b = rgb_data[i * 3 + 2];
        
        // ITU-R BT.709 conversion
        double gray = 0.2126 * r + 0.7152 * g + 0.0722 * b;
        gray_data[i] = (unsigned char)(gray + 0.5); // Round to nearest
    }
    
    // Save grayscale
    std::ofstream output(argv[2], std::ios::binary);
    output.write(reinterpret_cast<char*>(&gray_data[0]), 640 * 640);
    output.close();
    
    // Analyze the grayscale conversion
    unsigned char min_val = 255, max_val = 0;
    for (int i = 0; i < 640 * 640; i++) {
        if (gray_data[i] < min_val) min_val = gray_data[i];
        if (gray_data[i] > max_val) max_val = gray_data[i];
    }
    
    std::cout << "RGB->Grayscale conversion complete:" << std::endl;
    std::cout << "  Input: " << argv[1] << " (" << rgb_data.size() << " bytes)" << std::endl;
    std::cout << "  Output: " << argv[2] << " (" << gray_data.size() << " bytes)" << std::endl;
    std::cout << "  Grayscale range: " << (int)min_val << " - " << (int)max_val << std::endl;
    
    // Show first few pixels
    std::cout << "First 10 RGB->Gray conversions:" << std::endl;
    for (int i = 0; i < 10; i++) {
        unsigned char r = rgb_data[i * 3 + 0];
        unsigned char g = rgb_data[i * 3 + 1]; 
        unsigned char b = rgb_data[i * 3 + 2];
        std::cout << "  RGB(" << (int)r << "," << (int)g << "," << (int)b << ") -> " << (int)gray_data[i] << std::endl;
    }
    
    return 0;
}
