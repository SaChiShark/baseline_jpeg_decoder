#include "decoder/BitReader.hpp"

namespace Jpeg::Decoder {

    BitReader::BitReader(StreamReader& reader) : m_reader(reader), m_buffer(0), m_count(0) {}


    uint32_t BitReader::readBits(int n){
        //如果要求讀取 0 位元，直接回傳 0
        if(n == 0){
            return 0;
        }
        if(n > 16 || n < 0) throw std::runtime_error("illegal readBits requested");
        //如果buffer 資料不夠 就從檔案讀取
        while (m_count < n){
            uint8_t byte = m_reader.readByte();
            //檢查是否為marker
            if (byte == 0xFF){
                if(m_reader.readByte() != 0) throw std::runtime_error("Found Marker in Scan Data");
            }
            m_buffer = (m_buffer << 8) | byte;
            m_count += 8;
        }
        int offset = 32 - m_count;
        uint32_t result =  m_buffer << offset >> 32 - n;
        m_count -= n;
        return result;
    }

} // namespace Jpeg::Decoder
