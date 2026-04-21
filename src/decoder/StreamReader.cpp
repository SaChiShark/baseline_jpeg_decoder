#include "decoder/StreamReader.hpp"
#include <iostream>

namespace Jpeg::Decoder {

    StreamReader::StreamReader(const std::string& path) {
        m_file.open(path, std::ios::binary);
        if (!m_file.is_open()) {
            std::cerr << "Error: Could not open file " << path << std::endl;
        }
    }
    StreamReader::~StreamReader() {
        if (m_file.is_open()) {
            m_file.close();
        }
    }

    bool StreamReader::isOpen() const {
        return m_file.is_open();
    }

    bool StreamReader::isEOF() {
        return m_file.peek() == EOF;
    }

    uint8_t StreamReader::readByte() {
        uint8_t byte = 0;
        m_file.read(reinterpret_cast<char*>(&byte), 1);
        return byte;
    }

    uint16_t StreamReader::readWord() {
        // Big-Endian
        uint8_t high = readByte();
        uint8_t low  = readByte();
        return (static_cast<uint16_t>(high) << 8) | low;
    }

    void StreamReader::skip(std::streamsize length) {
        m_file.seekg(length, std::ios::cur);
    }

} // namespace Jpeg::Decoder
