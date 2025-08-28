#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t signature = 0x4D42;  // "BM"
    uint32_t fileSize;
    uint32_t reserved = 0;
    uint32_t dataOffset = 54;
    uint32_t headerSize = 40;
    int32_t width = 640;
    int32_t height = -640;  // NEGATIVE = top-down (fixes upside down issue)
    uint16_t planes = 1;
    uint16_t bitsPerPixel = 8;
    uint32_t compression = 0;
    uint32_t imageSize;
    int32_t xPixelsPerMeter = 2835;
    int32_t yPixelsPerMeter = 2835;
    uint32_t colorsUsed = 256;
    uint32_t colorsImportant = 256;
};
#pragma pack(pop)

class RawToBMP {
public:
    static bool convertGrayscaleRAWToBMP(const std::string& rawFile, const std::string& bmpFile) {
        // Read RAW file
        std::ifstream input(rawFile, std::ios::binary);
        if (!input) {
            std::cerr << "Cannot open RAW file: " << rawFile << std::endl;
            return false;
        }
        
        std::vector<uint8_t> rawData(640 * 640);
        input.read(reinterpret_cast<char*>(rawData.data()), 640 * 640);
        input.close();
        
        if (input.gcount() != 640 * 640) {
            std::cerr << "RAW file has wrong size: " << input.gcount() << " bytes" << std::endl;
            return false;
        }
        
        // Create BMP
        std::ofstream output(bmpFile, std::ios::binary);
        if (!output) {
            std::cerr << "Cannot create BMP file: " << bmpFile << std::endl;
            return false;
        }
        
        // Calculate sizes
        uint32_t rowSize = ((640 * 8 + 31) / 32) * 4;  // BMP rows must be 4-byte aligned
        uint32_t imageSize = rowSize * 640;
        uint32_t fileSize = 54 + 256 * 4 + imageSize;  // Header + palette + image
        
        // Write BMP header
        BMPHeader header;
        header.fileSize = fileSize;
        header.imageSize = imageSize;
        header.dataOffset = 54 + 256 * 4;  // Header + color palette
        
        output.write(reinterpret_cast<const char*>(&header), sizeof(header));
        
        // Write grayscale color palette (256 colors from black to white)
        for (int i = 0; i < 256; ++i) {
            uint8_t color[4] = {static_cast<uint8_t>(i), static_cast<uint8_t>(i), static_cast<uint8_t>(i), 0};
            output.write(reinterpret_cast<const char*>(color), 4);
        }
        
        // Write image data (now top-to-bottom since height is negative)
        std::vector<uint8_t> rowBuffer(rowSize, 0);
        for (int y = 0; y < 640; ++y) {  // Top-to-bottom (normal order)
            // Copy row data
            for (int x = 0; x < 640; ++x) {
                rowBuffer[x] = rawData[y * 640 + x];
            }
            output.write(reinterpret_cast<const char*>(rowBuffer.data()), rowSize);
        }
        
        output.close();
        
        std::cout << "✅ Converted " << rawFile << " → " << bmpFile << std::endl;
        std::cout << "   Open with Paint, Image Viewer, or any image editor!" << std::endl;
        
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "RAW to BMP Converter" << std::endl;
        std::cout << "Usage: " << argv[0] << " <input.raw> <output.bmp>" << std::endl;
        std::cout << "Example: " << argv[0] << " building_edges.raw building_edges.bmp" << std::endl;
        std::cout << "         " << argv[0] << " books_edges.raw books_edges.bmp" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    
    std::cout << "Converting 640x640 grayscale RAW to BMP..." << std::endl;
    
    if (RawToBMP::convertGrayscaleRAWToBMP(inputFile, outputFile)) {
        std::cout << "\n🎯 Success! Now you can:" << std::endl;
        std::cout << "1. Double-click " << outputFile << " to open in Windows Photo Viewer" << std::endl;
        std::cout << "2. Open with Paint to see edge detection results" << std::endl;
        std::cout << "3. White pixels = strong edges, Black = no edges" << std::endl;
    } else {
        std::cout << "\n❌ Conversion failed!" << std::endl;
        return 1;
    }
    
    return 0;
}
