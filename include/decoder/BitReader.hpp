#ifndef JPEG_BIT_READER_HPP
#define JPEG_BIT_READER_HPP

#include "decoder/StreamReader.hpp"

namespace Jpeg::Decoder {
    class BitReader {
    public:
        BitReader(StreamReader& reader);
        uint32_t readBits(int n);
    private:
        StreamReader& m_reader;
        uint32_t m_buffer;
        int m_count;
    };

}

#endif