# Sample Images

This directory contains sample images for testing the Sobel filter implementation.

## Usage

Place your 640x640 RGB images here for processing:

```bash
# Convert JPG to RAW format
.\bmp_to_raw.exe images\sample.bmp sample.raw

# Run Sobel filter
.\sobel_filter.exe sample.raw output.raw

# Convert back to viewable format
.\raw_to_bmp.exe output.raw output.bmp
```

## Image Requirements

- Format: BMP or JPG (for conversion)
- Dimensions: 640x640 pixels
- Color: RGB (24-bit)
- File size: ~1.2MB for raw RGB data

## Expected Output

The Sobel filter will detect edges in your images, producing grayscale edge maps suitable for computer vision applications.
