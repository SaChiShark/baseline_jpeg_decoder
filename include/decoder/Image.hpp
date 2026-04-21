#ifndef JPEG_IMAGE_HPP
#define JPEG_IMAGE_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include "decoder/Types.hpp"
#include "decoder/HuffmanDecoder.hpp"
#include "decoder/Dequantizer.hpp"
#include "decoder/IDCT.hpp"
#include "decoder/Upsampler.hpp"
#include "decoder/InverseZigZag.hpp"
#include "decoder/StreamReader.hpp"

namespace Jpeg::Decoder {

    class JpegImage {
    public:
        JpegImage();
        
        void setDimensions(uint16_t w, uint16_t h);
        void setQuantTable(uint8_t id, const std::array<uint16_t,64>& data);
        void addColorComponent(const ColorComponent& comp, uint8_t id);
        void setTdTa(uint8_t id, uint8_t Td, uint8_t Ta);
        void addHuffmanTable(uint8_t id, uint8_t type, const HuffmanTableSpec& spec);
        
        void render(StreamReader& reader);
        
        // 將內部數據 (YCbCr) 轉為 RGB 並輸出為 BMP 圖檔
        bool exportToBmp(const std::string& filename);

    private:
        void readMcu(BitReader& reader, int mcuX, int mcuY);
        void storeBlock(int c, int mcuX, int mcuY, int h, int v, const std::array<int16_t, 64>& block);

        //圖片長寬
        uint16_t m_width;
        uint16_t m_height;
        //量化表
        std::array<std::array<uint16_t, 64>, 4> m_qTables;
        //色彩分量
        std::array<ColorComponent, 3> m_components;
        //YUV資料
        std::vector<std::vector<std::vector<uint8_t>>> m_ycbcr;
        //huffman解碼器
        HuffmanDecoder m_huffmanDecoder;
        //dequantizer
        Dequantizer m_dequantizer;
        //inverse zigzag
        InverseZigZag m_inverseZigZag;
        //idct
        IDCT m_idct;
        //upsampler
        Upsampler m_upsampler;
    };

} // namespace Jpeg::Decoder

#endif
