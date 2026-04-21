#ifndef JPEG_DEQUANTIZER_HPP
#define JPEG_DEQUANTIZER_HPP

#include <cstdint>
#include <array>

namespace Jpeg::Decoder {

    class Dequantizer {
    public:
        void dequantize(std::array<int16_t, 64>& block, const uint16_t* table);
    };

} // namespace Jpeg::Decoder

#endif
