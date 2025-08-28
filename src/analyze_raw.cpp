#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <file.raw>" << std::endl;
        return 1;
    }
    
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file: " << argv[1] << std::endl;
        return 1;
    }
    
    std::vector<unsigned char> data(640 * 640);
    file.read(reinterpret_cast<char*>(&data[0]), 640 * 640);
    
    // Analyze the data
    unsigned char minVal = *std::min_element(data.begin(), data.end());
    unsigned char maxVal = *std::max_element(data.begin(), data.end());
    
    // Count non-zero pixels
    int nonZeroCount = 0;
    int zeroCount = 0;
    std::vector<int> histogram(256, 0);
    
    for (size_t i = 0; i < data.size(); i++) {
        unsigned char val = data[i];
        histogram[val]++;
        if (val > 0) nonZeroCount++;
        else zeroCount++;
    }
    
    std::cout << "=== RAW FILE ANALYSIS: " << argv[1] << " ===" << std::endl;
    std::cout << "File size: " << data.size() << " bytes" << std::endl;
    std::cout << "Min value: " << (int)minVal << std::endl;
    std::cout << "Max value: " << (int)maxVal << std::endl;
    std::cout << "Zero pixels: " << zeroCount << " (" << (100.0 * zeroCount / data.size()) << "%)" << std::endl;
    std::cout << "Non-zero pixels: " << nonZeroCount << " (" << (100.0 * nonZeroCount / data.size()) << "%)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Value distribution (showing values with >0.1% of pixels):" << std::endl;
    for (int i = 0; i < 256; i++) {
        if (histogram[i] > (int)(data.size() * 0.001)) {  // >0.1%
            std::cout << "  Value " << i << ": " << histogram[i] << " pixels (" 
                      << (100.0 * histogram[i] / data.size()) << "%)" << std::endl;
        }
    }
    
    // Show first few pixels
    std::cout << "\nFirst 20 pixel values: ";
    for (int i = 0; i < 20 && i < (int)data.size(); i++) {
        std::cout << (int)data[i] << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
