#ifndef JPEG_DECODER_STREAM_READER_HPP
#define JPEG_DECODER_STREAM_READER_HPP

#include <fstream>
#include <string>
#include <cstdint>

namespace Jpeg::Decoder {

    class StreamReader {
    public:
        explicit StreamReader(const std::string& path);
        ~StreamReader();

        StreamReader(const StreamReader&) = delete;
        StreamReader& operator=(const StreamReader&) = delete;

        bool isOpen() const;
        bool isEOF();

        // 核心讀取功能
        uint8_t  readByte();
        uint16_t readWord();
        void     skip(std::streamsize length);

    private:
        std::ifstream m_file;
    };

} // namespace Jpeg::Decoder

#endif