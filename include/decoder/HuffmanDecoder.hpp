#ifndef JPEG_HUFFMAN_DECODER_HPP
#define JPEG_HUFFMAN_DECODER_HPP

#include <cstdint>
#include <array>
#include "decoder/StreamReader.hpp"
#include "decoder/BitReader.hpp"
#include "decoder/Types.hpp"

namespace Jpeg::Decoder {

    class HuffmanDecoder {
    public:
        HuffmanDecoder();

        // 將解析到的 spec 存到內部 (為來如果要優化直接改 HuffmanDecoder就好)
        void loadTable(uint8_t id, uint8_t type, const HuffmanTableSpec& spec);
        
        // BitReader 讀資料並解碼一個 MCU Component Block
        void decodeBlock(BitReader& reader, int DC_id, int AC_id, int c, std::array<int16_t, 64>& block);

    private:
        int      decodeValue(uint32_t code, int len);
        uint8_t  matchHuffman(BitReader& reader, int table_id);
        
        // 紀錄每個 layer 的最小值和最大值，支援 4 張表
        std::array<std::array<int32_t, 16>, 4> m_layerMin;
        std::array<std::array<int32_t, 16>, 4> m_layerMax;
        // 紀錄每個 layer 在 huffval 中的起始偏移量
        std::array<std::array<int32_t, 16>, 4> m_offset;
        
        // 4 張 Huffman 表的符號值 (2 DC, 2 AC)
        std::array<std::array<uint8_t, 256>, 4> m_huffvals;
        
        // 紀錄 3 個顏色分量的最後一個 DC 值 (Y, Cb, Cr)
        std::array<int16_t, 3> m_lastDcValues;
    };

} // namespace Jpeg::Decoder

#endif
