#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include "../include/image.hpp"
#include "../include/image_io.hpp"

using namespace sobel;

class RawViewer {
public:
    static void ViewAsText(const std::string& rawPath, int sampleSize = 10) {
        auto result = ImageIO::readGrayscaleImage(rawPath);
        if (result.isError()) {
            std::cerr << "Failed to read raw file: " << result.getError() << std::endl;
            return;
        }
        
        GrayscaleImage image = result.getValue();
        size_t width = image.width();
        size_t height = image.height();
        
        std::cout << "Image size: " << width << "x" << height << std::endl;
        std::cout << "\nSample " << sampleSize << "x" << sampleSize << " pixels from center:" << std::endl;
        
        size_t startX = width / 2 - sampleSize / 2;
        size_t startY = height / 2 - sampleSize / 2;
        
        for (size_t y = startY; y < startY + sampleSize && y < height; ++y) {
            for (size_t x = startX; x < startX + sampleSize && x < width; ++x) {
                uint8_t pixel = image.getPixel(x, y);
                std::cout << std::setw(3) << static_cast<int>(pixel) << " ";
            }
            std::cout << std::endl;
        }
        
        // Statistics
        std::vector<int> histogram(256, 0);
        int minVal = 255, maxVal = 0;
        long long sum = 0;
        
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                uint8_t pixel = image.getPixel(x, y);
                histogram[pixel]++;
                minVal = std::min(minVal, static_cast<int>(pixel));
                maxVal = std::max(maxVal, static_cast<int>(pixel));
                sum += pixel;
            }
        }
        
        double average = static_cast<double>(sum) / (width * height);
        
        std::cout << "\nImage Statistics:" << std::endl;
        std::cout << "Min value: " << minVal << std::endl;
        std::cout << "Max value: " << maxVal << std::endl;
        std::cout << "Average: " << std::fixed << std::setprecision(2) << average << std::endl;
        
        // Count edge pixels (assuming edges are bright)
        int edgeCount = 0;
        for (int i = 128; i < 256; ++i) {
            edgeCount += histogram[i];
        }
        double edgePercentage = 100.0 * edgeCount / (width * height);
        std::cout << "Edge pixels (>127): " << edgeCount << " (" << std::fixed << std::setprecision(1) << edgePercentage << "%)" << std::endl;
        
        if (edgePercentage > 5) {
            std::cout << "✅ Good edge detection - reasonable amount of edges found!" << std::endl;
        } else if (edgePercentage > 1) {
            std::cout << "⚠️  Some edges detected - might be working" << std::endl;
        } else {
            std::cout << "❌ Very few edges - check if input image has clear features" << std::endl;
        }
    }
    
    static void SaveAsASCII(const std::string& rawPath, const std::string& txtPath) {
        auto result = ImageIO::readGrayscaleImage(rawPath);
        if (result.isError()) {
            std::cerr << "Failed to read raw file: " << result.getError() << std::endl;
            return;
        }
        
        GrayscaleImage image = result.getValue();
        std::ofstream file(txtPath);
        
        if (!file.is_open()) {
            std::cerr << "Failed to create output file: " << txtPath << std::endl;
            return;
        }
        
        file << "# Edge Detection Result - " << image.width() << "x" << image.height() << std::endl;
        file << "# Values: 0=no edge, 255=strong edge" << std::endl;
        
        // ASCII art representation
        const char* chars = " .:-=+*#%@";
        
        for (size_t y = 0; y < image.height(); y += 4) { // Sample every 4th row
            for (size_t x = 0; x < image.width(); x += 2) { // Sample every 2nd column
                uint8_t pixel = image.getPixel(x, y);
                int index = (pixel * 9) / 255;
                file << chars[index];
            }
            file << std::endl;
        }
        
        file.close();
        std::cout << "ASCII art saved to: " << txtPath << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <edges.raw> [ascii_output.txt]" << std::endl;
        std::cout << "Example: " << argv[0] << " edges.raw result.txt" << std::endl;
        return 1;
    }
    
    std::string rawPath = argv[1];
    
    std::cout << "Viewing edge detection results..." << std::endl;
    RawViewer::ViewAsText(rawPath, 20);
    
    if (argc >= 3) {
        std::string txtPath = argv[2];
        std::cout << "\nCreating ASCII art representation..." << std::endl;
        RawViewer::SaveAsASCII(rawPath, txtPath);
    }
    
    return 0;
}
