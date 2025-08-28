#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>

// Simple RAW grayscale image analyzer
class EdgeAnalyzer {
public:
    static void analyzeEdgeFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Cannot open: " << filename << std::endl;
            return;
        }
        
        // Read all pixels
        std::vector<uint8_t> pixels(640 * 640);
        file.read(reinterpret_cast<char*>(pixels.data()), 640 * 640);
        file.close();
        
        std::cout << "\n=== Edge Detection Analysis: " << filename << " ===" << std::endl;
        
        // Statistics
        auto minmax = std::minmax_element(pixels.begin(), pixels.end());
        int minVal = *minmax.first;
        int maxVal = *minmax.second;
        
        long long sum = 0;
        std::vector<int> histogram(256, 0);
        
        for (uint8_t pixel : pixels) {
            sum += pixel;
            histogram[pixel]++;
        }
        
        double average = static_cast<double>(sum) / pixels.size();
        
        // Count edge pixels (bright = edges)
        int strongEdges = 0, mediumEdges = 0, weakEdges = 0;
        for (int i = 0; i < 256; ++i) {
            if (i >= 200) strongEdges += histogram[i];       // Very bright
            else if (i >= 100) mediumEdges += histogram[i];  // Medium bright  
            else if (i >= 50) weakEdges += histogram[i];     // Somewhat bright
        }
        
        double strongPercent = 100.0 * strongEdges / pixels.size();
        double mediumPercent = 100.0 * mediumEdges / pixels.size();
        double weakPercent = 100.0 * weakEdges / pixels.size();
        
        std::cout << "Pixel value range: " << minVal << " - " << maxVal << std::endl;
        std::cout << "Average intensity: " << std::fixed << std::setprecision(1) << average << std::endl;
        std::cout << "Strong edges (200-255): " << strongPercent << "% (" << strongEdges << " pixels)" << std::endl;
        std::cout << "Medium edges (100-199): " << mediumPercent << "% (" << mediumEdges << " pixels)" << std::endl;
        std::cout << "Weak edges (50-99): " << weakPercent << "% (" << weakEdges << " pixels)" << std::endl;
        
        // Show sample 10x10 from center
        std::cout << "\nSample 10x10 from center (edge intensities):" << std::endl;
        int startX = 315, startY = 315; // Center of 640x640
        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 10; ++x) {
                int idx = (startY + y) * 640 + (startX + x);
                std::cout << std::setw(3) << static_cast<int>(pixels[idx]) << " ";
            }
            std::cout << std::endl;
        }
        
        // Interpretation
        std::cout << "\n=== INTERPRETATION ===" << std::endl;
        if (strongPercent > 5) {
            std::cout << "✅ EXCELLENT: Lots of strong edges detected!" << std::endl;
        } else if (strongPercent > 1) {
            std::cout << "✅ GOOD: Reasonable edge detection" << std::endl;
        } else if (mediumPercent > 5) {
            std::cout << "⚠️  OKAY: Some edges, but weak" << std::endl;
        } else if (maxVal > 50) {
            std::cout << "⚠️  WEAK: Few edges detected" << std::endl;
        } else {
            std::cout << "❌ PROBLEM: Almost no edges (all dark)" << std::endl;
        }
        
        if (maxVal == 255) {
            std::cout << "✅ Good dynamic range (uses full 0-255 scale)" << std::endl;
        }
        
        if (average < 20) {
            std::cout << "✅ Mostly background (expected for edge detection)" << std::endl;
        }
    }
    
    static void showASCIIPreview(const std::string& filename, int sampleRate = 8) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Cannot open: " << filename << std::endl;
            return;
        }
        
        std::vector<uint8_t> pixels(640 * 640);
        file.read(reinterpret_cast<char*>(pixels.data()), 640 * 640);
        file.close();
        
        std::cout << "\n=== ASCII Preview (sampled every " << sampleRate << " pixels) ===" << std::endl;
        const char* chars = " .:-=+*#%@";
        
        for (int y = 0; y < 640; y += sampleRate) {
            for (int x = 0; x < 640; x += sampleRate) {
                uint8_t pixel = pixels[y * 640 + x];
                int index = (pixel * 9) / 255;
                std::cout << chars[index];
            }
            std::cout << std::endl;
        }
        std::cout << "Legend: ' '=no edge, '@'=strong edge" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <edge_file.raw> [ascii]" << std::endl;
        std::cout << "Example: " << argv[0] << " building_edges.raw" << std::endl;
        std::cout << "         " << argv[0] << " building_edges.raw ascii" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    bool showASCII = (argc >= 3 && std::string(argv[2]) == "ascii");
    
    EdgeAnalyzer::analyzeEdgeFile(filename);
    
    if (showASCII) {
        EdgeAnalyzer::showASCIIPreview(filename);
    }
    
    return 0;
}
