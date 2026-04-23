#include "decoder/HuffmanDecoder.hpp"
#include <iostream>
namespace Jpeg::Decoder {

    HuffmanDecoder::HuffmanDecoder() {
        // Initialize m_lastDcValues
        for (int i = 0; i < 3; ++i) {
            m_lastDcValues[i] = 0;
        }
    }

    void HuffmanDecoder::loadTable(uint8_t id, uint8_t type, const HuffmanTableSpec& spec) {
        // DC (0)  AC (1)
        int internal_id = (type << 1) | id;
        if (internal_id >= 4) return;

        m_huffvals[internal_id] = spec.huffval;

        int offset = 0;
        int32_t min_code = 0;

        for (int i = 0; i < 16; i++) {
            if (spec.bits[i] == 0) {
                m_layerMin[internal_id][i] = -1;
                m_layerMax[internal_id][i] = -1;
                m_offset[internal_id][i] = -1;
            } else {
                m_layerMin[internal_id][i] = min_code;
                m_layerMax[internal_id][i] = min_code + spec.bits[i] - 1;
                m_offset[internal_id][i] = offset;

                offset += spec.bits[i];
                min_code += spec.bits[i];
            }
            min_code <<= 1;
        }
    }

    void HuffmanDecoder::decodeBlock(BitReader& reader, int DC_id, int AC_id, int c, std::array<int16_t, 64>& block) {
        // 轉換為內部 0-3 的索引
        DC_id = (0 << 1) | DC_id;
        AC_id = (1 << 1) | AC_id;

        // 1. 解碼 DC
        uint8_t value_len = matchHuffman(reader, DC_id);
        uint32_t diff_bits = reader.readBits(value_len);
        int diff = decodeValue(diff_bits, value_len);
        int DC = m_lastDcValues[c] + diff;
        block[0] = DC;
        m_lastDcValues[c] = DC;

        // 2. 解碼 AC
        int i = 1; 
        while (i < 64) {
            uint8_t AC_info = matchHuffman(reader, AC_id);
            //End of block
            if (AC_info == 0x00) { 
                while (i < 64) block[i++] = 0;
                break;
            }
            //Padding 0 * 16
            if (AC_info == 0xF0) { 
                for (int j = 0; j < 16 && i < 64; j++) block[i++] = 0;
                continue;
            }
            // 正常 AC decode
            uint8_t leeding_zeros = AC_info >> 4;
            value_len = AC_info & 0x0F;

            for (int j = 0; j < leeding_zeros && i < 64; j++) block[i++] = 0;
        
            if (i < 64) {
                uint32_t val_bits = reader.readBits(value_len);
                block[i] = decodeValue(val_bits, value_len);
                i++;
            }
        }
    }

    int HuffmanDecoder::decodeValue(uint32_t bits, int len) {
        if (len == 0) return 0;
        
        if (bits & (1 << (len - 1))) {
            return (int)bits;
        } else {
            return (int)bits - ((1 << len) - 1);
        }
    }
    
    uint8_t HuffmanDecoder::matchHuffman(BitReader& reader, int table_id) {
        if (table_id >= 4) throw std::runtime_error("Invalid Huffman table ID");

        uint16_t code = 0;
        //一層一層比
        for (int i = 0; i < 16; i++) {
            code = (code << 1) | reader.readBits(1);
            //是否在這層的範圍
            if (code >= m_layerMin[table_id][i] && code <= m_layerMax[table_id][i]) {
                int val_index = m_offset[table_id][i] + (code - m_layerMin[table_id][i]);
                return m_huffvals[table_id][val_index];
            }
        }
        throw std::runtime_error("Illegal Huffman code detected");
    }

} // namespace Jpeg::Decoder
