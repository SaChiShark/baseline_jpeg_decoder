#ifndef JPEG_CONSTANTS_HPP
#define JPEG_CONSTANTS_HPP

#include <cstdint>

namespace Jpeg::Constants {
    //Markers
    constexpr uint8_t MARKER_PREFIX    = 0xFF; // 所有 Marker 的前綴
    constexpr uint8_t SOI      = 0xD8; // Start of Image
    constexpr uint8_t EOI      = 0xD9; // End of Image
    constexpr uint8_t DQT      = 0xDB; // Define Quantization Table
    constexpr uint8_t DHT      = 0xC4; // Define Huffman Table
    constexpr uint8_t SOF0     = 0xC0; // Start of Frame (Baseline DCT)
    constexpr uint8_t SOS      = 0xDA; // Start of Scan
    constexpr uint8_t APP0     = 0xE0; // JFIF 應用程式區段 (通常包含解析度資訊)
    constexpr int BlockSize = 8; 

} // namespace Jpeg::Constants

#endif