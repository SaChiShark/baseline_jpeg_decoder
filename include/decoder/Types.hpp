#ifndef JPEG_TYPES_HPP
#define JPEG_TYPES_HPP

#include <array>
#include <cstdint>

namespace Jpeg::Decoder {
    struct HuffmanTableSpec {
        std::array<uint8_t, 16> bits;
        std::array<uint8_t, 256> huffval;
    };

    struct ColorComponent {
        uint8_t H = 0; // Horizontal sampling factor
        uint8_t V = 0; // Vertical sampling factor
        uint8_t Tq = 0; // Quantization table
        uint8_t Td = 0; // DC table
        uint8_t Ta = 0; // AC table
    };


} // namespace Jpeg::Decoder

#endif
