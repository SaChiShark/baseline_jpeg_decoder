#include "decoder/Upsampler.hpp"

namespace Jpeg::Decoder {

    std::vector<std::vector<uint8_t>> Upsampler::upsample(const std::vector<std::vector<uint8_t>>& input, int targetW, int targetH) {
        int srcH = input.size();
        int srcW = input[0].size();

        std::vector<std::vector<uint8_t>> output(targetH, std::vector<uint8_t>(targetW, 239));

        if (srcH == 0 || srcW == 0) return output;

        for (int y = 0; y < targetH; ++y) {
            for (int x = 0; x < targetW; ++x) {
                int srcY = (y * srcH) / targetH;
                int srcX = (x * srcW) / targetW;
                
                output[y][x] = input[srcY][srcX];
            }
        }

        return output;
    }

} // namespace Jpeg::Decoder
