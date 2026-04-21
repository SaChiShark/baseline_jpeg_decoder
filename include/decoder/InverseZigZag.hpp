#ifndef JPEG_INVERSE_ZIGZAG_HPP
#define JPEG_INVERSE_ZIGZAG_HPP

#include <cstdint>
#include <array>

namespace Jpeg::Decoder {

    class InverseZigZag {
    public:
        void transform(std::array<int16_t, 64>& block);
    };

} // namespace Jpeg::Decoder

#endif
