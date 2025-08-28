#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input.raw> <output.raw>" << std::endl;
        return 1;
    }
    
    std::ifstream input(argv[1], std::ios::binary);
    std::vector<unsigned char> data(640 * 640);
    input.read(reinterpret_cast<char*>(&data[0]), 640 * 640);
    input.close();
    
    // Find min/max
    unsigned char minVal = *std::min_element(data.begin(), data.end());
    unsigned char maxVal = *std::max_element(data.begin(), data.end());
    
    std::cout << "Original range: " << (int)minVal << " - " << (int)maxVal << std::endl;
    
    // Method 1: Simple threshold
    std::vector<unsigned char> method1 = data;
    for (size_t i = 0; i < method1.size(); i++) {
        if (method1[i] < 50) method1[i] = 0;        // Background -> black
        else if (method1[i] > 150) method1[i] = 255; // Strong edges -> white
        else method1[i] = method1[i] * 2;           // Weak edges -> amplify
    }
    
    // Method 2: Normalize to full range
    std::vector<unsigned char> method2 = data;
    if (maxVal > minVal) {
        double scale = 255.0 / (maxVal - minVal);
        for (size_t i = 0; i < method2.size(); i++) {
            double normalized = (method2[i] - minVal) * scale;
            method2[i] = (unsigned char)std::min(255.0, std::max(0.0, normalized));
        }
    }
    
    // Save method 1 (threshold)
    std::string output1 = std::string(argv[2]) + "_threshold.raw";
    std::ofstream out1(output1.c_str(), std::ios::binary);
    out1.write(reinterpret_cast<char*>(&method1[0]), 640 * 640);
    out1.close();
    
    // Save method 2 (normalize)
    std::string output2 = std::string(argv[2]) + "_normalized.raw";
    std::ofstream out2(output2.c_str(), std::ios::binary);
    out2.write(reinterpret_cast<char*>(&method2[0]), 640 * 640);
    out2.close();
    
    std::cout << "Created:" << std::endl;
    std::cout << "  " << output1 << " (threshold method)" << std::endl;
    std::cout << "  " << output2 << " (normalize method)" << std::endl;
    
    return 0;
}
