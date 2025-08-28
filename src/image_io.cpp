/**
 * @file image_io.cpp
 * @brief Implementation of image I/O operations
 * @author BK Park
 * @version 1.0.0
 * @date 2025-08-27
 */

#include "image_io.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace sobel {

Result<RGBImage> 
ImageIO::loadRGBImage(const std::string& filepath, std::size_t width, std::size_t height) {
    // Validate file exists
    if (!std::filesystem::exists(filepath)) {
        return Result<RGBImage>(ImageIOError::FileNotFound);
    }
    
    // Validate file size
    if (!validateRGBFileSize(filepath, width, height)) {
        return Result<RGBImage>(ImageIOError::InvalidFileSize);
    }
    
    // Open file for binary reading
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return Result<RGBImage>(ImageIOError::ReadError);
    }
    
    // Read RGB data
    const std::size_t expected_bytes = width * height * 3;
    std::vector<uint8_t> raw_data(expected_bytes);
    
    file.read(reinterpret_cast<char*>(raw_data.data()), expected_bytes);
    if (file.gcount() != static_cast<std::streamsize>(expected_bytes)) {
        return Result<RGBImage>(ImageIOError::ReadError);
    }
    
    // Convert raw bytes to RGB pixels
    std::vector<RGBPixel> pixels;
    pixels.reserve(width * height);
    
    for (std::size_t i = 0; i < expected_bytes; i += 3) {
        pixels.emplace_back(raw_data[i], raw_data[i + 1], raw_data[i + 2]);
    }
    
    try {
        return Result<RGBImage>(RGBImage(std::move(pixels), width, height));
    } catch (const std::exception&) {
        return Result<RGBImage>(ImageIOError::InvalidDimensions);
    }
}

Result<bool> 
ImageIO::saveGrayscaleImage(const GrayscaleImage& image, const std::string& filepath) {
    if (image.empty()) {
        return Result<bool>(ImageIOError::InvalidDimensions);
    }
    
    // Create output directory if it doesn't exist
    std::filesystem::path path(filepath);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    // Open file for binary writing
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return Result<bool>(ImageIOError::WriteError);
    }
    
    // Write grayscale data
    file.write(reinterpret_cast<const char*>(image.data()), image.size());
    
    if (!file.good()) {
        return Result<bool>(ImageIOError::WriteError);
    }
    
    return Result<bool>(true);
}

bool ImageIO::validateRGBFileSize(const std::string& filepath, 
                                 std::size_t expected_width, 
                                 std::size_t expected_height) {
    const std::size_t expected_size = expected_width * expected_height * 3;
    return getFileSize(filepath) == expected_size;
}

std::size_t ImageIO::getFileSize(const std::string& filepath) {
    try {
        return std::filesystem::file_size(filepath);
    } catch (const std::filesystem::filesystem_error&) {
        return 0;
    }
}

std::string toString(ImageIOError error) {
    switch (error) {
        case ImageIOError::FileNotFound:
            return "File not found";
        case ImageIOError::InvalidFileSize:
            return "Invalid file size for specified image dimensions";
        case ImageIOError::ReadError:
            return "Error reading from file";
        case ImageIOError::WriteError:
            return "Error writing to file";
        case ImageIOError::InvalidDimensions:
            return "Invalid image dimensions";
        default:
            return "Unknown error";
    }
}

} // namespace sobel
