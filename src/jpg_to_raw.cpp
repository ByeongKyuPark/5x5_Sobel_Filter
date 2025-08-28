#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <gdiplus.h>
#include "../include/image.hpp"
#include "../include/image_io.hpp"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
using namespace sobel;

class JPGConverter {
public:
    static bool Initialize() {
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        return GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) == Ok;
    }
    
    static bool ConvertJPGToRaw(const std::string& jpgPath, const std::string& rawPath) {
        // Convert string to wide string for GDI+
        std::wstring wPath(jpgPath.begin(), jpgPath.end());
        
        // Load the image
        Bitmap* bitmap = new Bitmap(wPath.c_str());
        if (bitmap->GetLastStatus() != Ok) {
            std::cerr << "Failed to load image: " << jpgPath << std::endl;
            delete bitmap;
            return false;
        }
        
        UINT width = bitmap->GetWidth();
        UINT height = bitmap->GetHeight();
        
        std::cout << "Loaded image: " << width << "x" << height << std::endl;
        
        // Check if we need to resize to 640x640
        if (width != 640 || height != 640) {
            std::cout << "Resizing from " << width << "x" << height << " to 640x640..." << std::endl;
            Bitmap* resized = new Bitmap(640, 640, PixelFormat24bppRGB);
            Graphics graphics(resized);
            graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            graphics.DrawImage(bitmap, 0, 0, 640, 640);
            
            delete bitmap;
            bitmap = resized;
        }
        
        // Create RGB image
        RGBImage rgbImage(640, 640);
        
        // Extract pixels
        BitmapData bitmapData;
        Rect rect(0, 0, 640, 640);
        bitmap->LockBits(&rect, ImageLockModeRead, PixelFormat24bppRGB, &bitmapData);
        
        BYTE* pixels = (BYTE*)bitmapData.Scan0;
        int stride = bitmapData.Stride;
        
        for (int y = 0; y < 640; ++y) {
            for (int x = 0; x < 640; ++x) {
                BYTE* pixel = pixels + y * stride + x * 3;
                // GDI+ uses BGR format, convert to RGB
                uint8_t b = pixel[0];
                uint8_t g = pixel[1];
                uint8_t r = pixel[2];
                
                rgbImage.setPixel(x, y, RGBPixel(r, g, b));
            }
        }
        
        bitmap->UnlockBits(&bitmapData);
        delete bitmap;
        
        // Save as raw
        auto result = ImageIO::saveGrayscaleImage(rgbImage, rawPath);
        if (result.hasError()) {
            std::cerr << "Failed to save raw file: " << toString(result.getError()) << std::endl;
            return false;
        }
        
        std::cout << "Successfully converted to: " << rawPath << std::endl;
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input.jpg> <output.raw>" << std::endl;
        std::cout << "Example: " << argv[0] << " ../images/1.jpg test_input.raw" << std::endl;
        return 1;
    }
    
    if (!JPGConverter::Initialize()) {
        std::cerr << "Failed to initialize GDI+" << std::endl;
        return 1;
    }
    
    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    
    std::cout << "Converting JPG to RAW format..." << std::endl;
    std::cout << "Input: " << inputPath << std::endl;
    std::cout << "Output: " << outputPath << std::endl;
    
    if (JPGConverter::ConvertJPGToRaw(inputPath, outputPath)) {
        std::cout << "\n✅ Conversion successful!" << std::endl;
        std::cout << "Now you can run: sobel_filter.exe " << outputPath << " edges.raw" << std::endl;
    } else {
        std::cout << "\n❌ Conversion failed!" << std::endl;
        return 1;
    }
    
    return 0;
}
