#include "decoder/IDCT.hpp"
#include <cmath>
#include <array>

namespace Jpeg::Decoder {
    //Come from AI
    // 預先計算的 IDCT 權重表 (Cosine Table)
    // 為了清晰與精度，我們使用靜態生成的常數
    static const double PI = 3.14159265358979323846;
    
    // 1D-IDCT 核心：將 8 個頻率係數轉換為 8 個空間數值
    static void idct1D(const double* in, double* out) {
        for (int i = 0; i < 8; ++i) {
            double sum = 0;
            for (int k = 0; k < 8; ++k) {
                double a = (k == 0) ? (1.0 / std::sqrt(2.0)) : 1.0;
                sum += a * in[k] * std::cos((2.0 * i + 1.0) * k * PI / 16.0);
            }
            out[i] = sum * 0.5; // 歸一化係數 1/2
        }
    }

    void IDCT::transform(std::array<int16_t, 64>& block) {
        double temp[64];     // 第一階段 (橫列) 轉換後的中間緩衝區
        double finalBlock[64]; // 第二階段 (縱列) 轉換後的緩衝區

        // 1. Row-wise IDCT
        for (int y = 0; y < 8; ++y) {
            double rowIn[8];
            double rowOut[8];
            for (int x = 0; x < 8; ++x) rowIn[x] = (double)block[y * 8 + x];
            
            idct1D(rowIn, rowOut);
            
            for (int x = 0; x < 8; ++x) temp[y * 8 + x] = rowOut[x];
        }

        // 2. Column-wise IDCT
        for (int x = 0; x < 8; ++x) {
            double colIn[8];
            double colOut[8];
            for (int y = 0; y < 8; ++y) colIn[y] = temp[y * 8 + x];
            
            idct1D(colIn, colOut);
            
            for (int y = 0; y < 8; ++y) finalBlock[y * 8 + x] = colOut[y];
        }

        // 3. 轉回 int16_t 並原地更新 block
        for (int i = 0; i < 64; ++i) {
            block[i] = (int16_t)std::round(finalBlock[i]);
        }
    }

} // namespace Jpeg::Decoder
