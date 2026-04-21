#include "decoder/Decoder.hpp"
#include "decoder/Constants.hpp"
#include "decoder/Image.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <array>

namespace Jpeg::Decoder {

    JpegDecoder::JpegDecoder() {
        m_handlers.fill(nullptr);

        // 列印 Marker
        m_handlers[Jpeg::Constants::SOI] = [](StreamReader&, JpegImage&) {
            std::cout << "[Marker] Start of Image (SOI)" << std::endl;
        };

        m_handlers[Jpeg::Constants::APP0] = [](StreamReader&, JpegImage&) {
            std::cout << "[Marker] Application Segment 0 (APP0)" << std::endl;
        };

        // 核心解析邏輯
        m_handlers[Jpeg::Constants::DQT] = [this](StreamReader& r, JpegImage& img) {
            std::cout << "[Marker] Define Quantization Table (DQT)" << std::endl;
            this->parseDQT(r, img);
        };
        m_handlers[Jpeg::Constants::DHT] = [this](StreamReader& r, JpegImage& img) {
            std::cout << "[Marker] Define Huffman Table (DHT)" << std::endl;
            this->parseDHT(r, img);
        };
        m_handlers[Jpeg::Constants::SOF0] = [this](StreamReader& r, JpegImage& img) {
            std::cout << "[Marker] Start of Frame (SOF0)" << std::endl;
            this->parseSOF(r, img);
        };
        m_handlers[Jpeg::Constants::SOS] = [this](StreamReader& r, JpegImage& img) {
            std::cout << "[Marker] Start of Scan (SOS)" << std::endl;
            this->parseSOS(r, img);
            img.render(r); // 解析完 SOS 立刻開始 render
        };
    }

    std::unique_ptr<JpegImage> JpegDecoder::decode(const std::string& filename) {
        StreamReader reader(filename);
        if (!reader.isOpen()) return nullptr;
        
        auto image = std::make_unique<JpegImage>();

        while (!reader.isEOF()) {
            uint8_t byte = reader.readByte();

            // 尋找 Marker 前綴 0xFF
            if (byte == Jpeg::Constants::MARKER_PREFIX) {
                uint8_t marker = reader.readByte();

                if (marker == Jpeg::Constants::EOI) {
                    std::cout << "End of Image (EOI)" << std::endl;
                    return image;
                }

                if (m_handlers[marker]) {
                    m_handlers[marker](reader, *image);
                } else {
                    if(marker == Jpeg::Constants::SOI){
                        std::cout << "Start of Image (SOI)" << std::endl;
                    }
                    // 遇到不認識的 Marker
                    else if (marker != Jpeg::Constants::SOI && marker != 0x00) {
                        uint16_t len = reader.readWord();
                        reader.skip(len - 2);
                    }
                }
            }
        }
        return image;
    }

    void JpegDecoder::parseDQT(StreamReader& reader, JpegImage& image) {
        uint16_t Lq = reader.readWord();
        int remaining = Lq - 2;
        // 有多個Table
        while (remaining > 0) {
            // Pq:  Precision – 0 = 8-bit, 1 = 16-bit
            // Tq:  Quantization table destination identifier
            uint8_t info = reader.readByte(); 
            remaining -= 1;
            uint8_t Tq = info & 0x0F;
            uint8_t Pq = info >> 4;
            std::cout << "  Table ID: " << (int)Tq << std::endl;

            std::array<uint16_t,64> table;
            for (int i = 0; i < 64; ++i) {
                if (Pq == 0) {
                    table[i] = reader.readByte();
                    remaining -= 1;
                } else {
                    table[i] = reader.readWord();
                    remaining -= 2;
                }
            }
            image.setQuantTable(Tq, table);
        }
    }

    void JpegDecoder::parseDHT(StreamReader& reader, JpegImage& image) {
        uint16_t Lh = reader.readWord();
        int remaining = Lh - 2;
        // 有多個Table
        while (remaining > 0) {
            // Tc:  Table class – 0 = DC table, 1 = AC table.
            // Th:  Huffman table destination identifier
            uint8_t info = reader.readByte();
            remaining -= 1;
            uint8_t Tc = info >> 4;    // 高 4 位元是 Table Class (0=DC, 1=AC)
            uint8_t Th = info & 0x0F;  // 低 4 位元是 Table ID
            std::cout << "  Table class: " << (int)Tc << std::endl;
            std::cout << "  Table ID: " << (int)Th << std::endl;

            HuffmanTableSpec spec;
            int totalSymbols = 0;
            for (int i = 0; i < 16; ++i) {
                uint8_t count = reader.readByte();
                spec.bits[i] = count;
                totalSymbols += count;
            }
            remaining -= 16;
            
            for (int i = 0; i < totalSymbols; ++i) {
                spec.huffval[i] = reader.readByte();
            }
            remaining -= totalSymbols;

            image.addHuffmanTable(Th, Tc, spec);
        }
    }

    void JpegDecoder::parseSOF(StreamReader& reader, JpegImage& image) {
        uint16_t Lf = reader.readWord(); // 跳過長度
        reader.readByte(); // 跳過精度 
        uint16_t height = reader.readWord();
        uint16_t width = reader.readWord();
        image.setDimensions(width, height);
        std::cout << "  Image Size: " << width << "x" << height << std::endl;
        
        uint8_t Nf = reader.readByte(); // 圖像中使用的component數量
        for(int i = 0; i < Nf; ++i) {
            ColorComponent comp;
            uint8_t id = reader.readByte(); // (1=Y, 2=Cb, 3=Cr)
            uint8_t Info = reader.readByte();
            comp.H = Info >> 4;
            comp.V = Info & 0x0F;
            comp.Tq = reader.readByte();
            comp.Td = 0;
            comp.Ta = 0;
            
            image.addColorComponent(comp, id);
        }
    }

    void JpegDecoder::parseSOS(StreamReader& reader, JpegImage& image) {
        uint16_t Ls = reader.readWord(); // SOS 區段總長度
        uint8_t Ns = reader.readByte(); // 掃描中的組件數量
        std::cout << "  Number of Components: " << (int)Ns << std::endl;
        
        for (int i = 0; i < Ns; ++i) {
            uint8_t Cs = reader.readByte();   // Cs: (1=Y, 2=Cb, 3=Cr)
            uint8_t Info = reader.readByte();
            uint8_t Td = (Info >> 4) & 0x0F; 
            uint8_t Ta = Info & 0x0F;
            image.setTdTa(Cs, Td, Ta);
        }
        reader.readByte(); // Ss
        reader.readByte(); // Se
        reader.readByte(); // Ah/Al

    }
    
} // namespace Jpeg::Decoder