/**
 * @file image_io.hpp
 * @brief Image I/O operations for raw binary files
 * @author BK Park
 * @version 1.0.0
 * @date 2025-08-27
 */

#pragma once

#include "image.hpp"
#include <string>
#include <optional>

namespace sobel {

/**
 * @brief Error codes for image I/O operations
 */
enum class ImageIOError {
    FileNotFound,
    InvalidFileSize,
    ReadError,
    WriteError,
    InvalidDimensions
};

/**
 * @brief Result type for image I/O operations
 */
template<typename T>
struct Result {
    std::optional<T> value;
    std::optional<ImageIOError> error;
    
    Result(T&& val) : value(std::move(val)) {}
    Result(const T& val) : value(val) {}
    Result(ImageIOError err) : error(err) {}
    
    bool hasValue() const { return value.has_value(); }
    bool hasError() const { return error.has_value(); }
    
    const T& getValue() const { return value.value(); }
    T& getValue() { return value.value(); }
    ImageIOError getError() const { return error.value(); }
    
    operator bool() const { return hasValue(); }
};

/**
 * @brief Image I/O utility class for raw binary files
 */
class ImageIO {
public:
    /**
     * @brief Load RGB image from raw binary file
     * @param filepath Path to input file
     * @param width Expected image width
     * @param height Expected image height
     * @return Result containing RGBImage or error
     */
    static Result<RGBImage> 
    loadRGBImage(const std::string& filepath, std::size_t width, std::size_t height);
    
    /**
     * @brief Save grayscale image to raw binary file
     * @param image Grayscale image to save
     * @param filepath Output file path
     * @return Result containing success or error
     */
    static Result<bool> 
    saveGrayscaleImage(const GrayscaleImage& image, const std::string& filepath);
    
    /**
     * @brief Validate file size for RGB image
     * @param filepath Path to file
     * @param expected_width Expected image width
     * @param expected_height Expected image height
     * @return True if file size matches expected RGB image size
     */
    static bool validateRGBFileSize(const std::string& filepath, 
                                   std::size_t expected_width, 
                                   std::size_t expected_height);
    
    /**
     * @brief Get file size in bytes
     * @param filepath Path to file
     * @return File size or 0 if file doesn't exist
     */
    static std::size_t getFileSize(const std::string& filepath);
    
private:
    ImageIO() = delete; // Static class
};

/**
 * @brief Convert ImageIOError to human-readable string
 * @param error Error code
 * @return Error description
 */
std::string toString(ImageIOError error);

} // namespace sobel
