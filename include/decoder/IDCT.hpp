#ifndef JPEG_IDCT_HPP
#define JPEG_IDCT_HPP

#include <cstdint>
#include <array>

namespace Jpeg::Decoder {

    class IDCT {
    public:
        void transform(std::array<int16_t, 64>& block);
    };

} // namespace Jpeg::Decoder

#endif
