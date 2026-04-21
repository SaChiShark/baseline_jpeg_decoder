#ifndef JPEG_UPSAMPLER_HPP
#define JPEG_UPSAMPLER_HPP

#include <vector>
#include <cstdint>

namespace Jpeg::Decoder {

    class Upsampler {
    public:
        std::vector<std::vector<uint8_t>> upsample( const std::vector<std::vector<uint8_t>>& input, 
            int targetW, 
            int targetH
        );
    };

} // namespace Jpeg::Decoder

#endif
