#include "decoder/Dequantizer.hpp"

namespace Jpeg::Decoder {

    void Dequantizer::dequantize(std::array<int16_t, 64>& block, const uint16_t* table) {
        for (int i = 0; i < 64; ++i) {
            block[i] = block[i] * table[i];
        }
    }

} // namespace Jpeg::Decoder
