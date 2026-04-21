#include "decoder/Image.hpp"
#include <iostream>
namespace Jpeg::Decoder {

    JpegImage::JpegImage() : m_width(0), m_height(0) {}
    
    void JpegImage::setDimensions(uint16_t w, uint16_t h) {
        m_width = w;
        m_height = h;
    }
    
    void JpegImage::setQuantTable(uint8_t id, const std::array<uint16_t,64>& data) {
        m_qTables[id] = data;
    }
    
    void JpegImage::addColorComponent(const ColorComponent& comp, uint8_t id) {
        m_components[id - 1] = comp;
    }

    void JpegImage::setTdTa(uint8_t id, uint8_t Td, uint8_t Ta) {
        m_components[id - 1].Td = Td;
        m_components[id - 1].Ta = Ta;
    }
    
    void JpegImage::addHuffmanTable(uint8_t id, uint8_t type, const HuffmanTableSpec& spec) {
        m_huffmanDecoder.loadTable(id, type, spec);
    }
    
    void JpegImage::render(StreamReader& reader) {
        BitReader bitReader(reader);
        int maxH = 0, maxV = 0;
        for (int i = 0; i < 3; ++i) {
            if (m_components[i].H > maxH) maxH = (int)m_components[i].H;
            if (m_components[i].V > maxV) maxV = (int)m_components[i].V;
        }
        // 計算 MCU 的數量 (Padding)
        int mcusX = (m_width + maxH * 8 - 1) / (maxH * 8);
        int mcusY = (m_height + maxV * 8 - 1) / (maxV * 8);

        //INIT buffer
        m_ycbcr.resize(3);
        for (int c = 0; c < 3; ++c) {
            int compWidth = mcusX * m_components[c].H * 8;
            int compHeight = mcusY * m_components[c].V * 8;

            m_ycbcr[c].assign(compHeight, std::vector<uint8_t>(compWidth, 0));
        }


        for (int mcuY = 0; mcuY < mcusY; ++mcuY) {
            for (int mcuX = 0; mcuX < mcusX; ++mcuX) {
                readMcu(bitReader, mcuX, mcuY);
            }
        }
        int fullWidth = mcusX * maxH * 8;
        int fullHeight = mcusY * maxV * 8;

        for (int c = 0; c < 3; ++c) {
            m_ycbcr[c] = m_upsampler.upsample(m_ycbcr[c], fullWidth, fullHeight);
        }

    }

    void JpegImage::readMcu(BitReader& reader, int mcuX, int mcuY) {
        for (int c = 0; c < 3; c++) {
            for (int v = 0; v < m_components[c].V; v++) {
                for (int h = 0; h < m_components[c].H; h++) {
                    std::array<int16_t, 64> block = {0};
                    m_huffmanDecoder.decodeBlock(reader, m_components[c].Td, m_components[c].Ta, c, block);
                    m_dequantizer.dequantize(block, m_qTables[m_components[c].Tq].data());
                    m_inverseZigZag.transform(block);
                    m_idct.transform(block); 
                    storeBlock(c, mcuX, mcuY, h, v, block);
                }
            }
        }
    }

    void JpegImage::storeBlock(int c, int mcuX, int mcuY, int h, int v, const std::array<int16_t, 64>& block) {
        // 計算該 8x8 Block 的位置
        int globalX = (mcuX * m_components[c].H + h) * 8;
        int globalY = (mcuY * m_components[c].V + v) * 8;

        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                // bias +128 將值轉為 0-255 範圍
                int val = block[row * 8 + col] + 128;
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                
                m_ycbcr[c][globalY + row][globalX + col] = (uint8_t)val;
            }
        }
    }
    
    bool JpegImage::exportToBmp(const std::string& filename) {
        //Come from AI
        if (m_ycbcr.empty() || m_ycbcr[0].empty()) return false;

        int height = m_ycbcr[0].size();
        int width = m_ycbcr[0][0].size();

        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) return false;

        // 1. BMP 檔案標頭準備
        int padding = (4 - (width * 3) % 4) % 4;
        uint32_t fileSize = 54 + (width * 3 + padding) * height;

#pragma pack(push, 1)
        struct {
            uint16_t type{0x4D42}; // "BM"
            uint32_t size;
            uint16_t res1{0}, res2{0};
            uint32_t offset{54};
        } fileHeader;
        fileHeader.size = fileSize;

        struct {
            uint32_t size{40};
            int32_t  width;
            int32_t  height;
            uint16_t planes{1};
            uint16_t bitCount{24};
            uint32_t compression{0};
            uint32_t imageSize{0};
            int32_t  xPels{0}, yPels{0};
            uint32_t clrUsed{0}, clrImp{0};
        } infoHeader;
        infoHeader.width = width;
        infoHeader.height = height;
#pragma pack(pop)

        ofs.write(reinterpret_cast<char*>(&fileHeader), 14);
        ofs.write(reinterpret_cast<char*>(&infoHeader), 40);

        // 2. 像素數據轉換與寫入 (BMP 由下往上儲存)
        std::vector<uint8_t> paddingBytes(3, 0);
        for (int y = height - 1; y >= 0; --y) {
            for (int x = 0; x < width; ++x) {
                float Y  = (float)m_ycbcr[0][y][x];
                float Cb = (float)m_ycbcr[1][y][x];
                float Cr = (float)m_ycbcr[2][y][x];

                // YCbCr 轉 RGB (ITU-R BT.601)
                int r = (int)(Y + 1.402f * (Cr - 128.0f));
                int g = (int)(Y - 0.34414f * (Cb - 128.0f) - 0.71414f * (Cr - 128.0f));
                int b = (int)(Y + 1.772f * (Cb - 128.0f));

                // 限幅 (Clamping)
                auto clamp = [](int v) -> uint8_t {
                    return (uint8_t)(v < 0 ? 0 : (v > 255 ? 255 : v));
                };

                uint8_t bgr[3] = { clamp(b), clamp(g), clamp(r) };
                ofs.write(reinterpret_cast<char*>(bgr), 3);
            }
            // 每一行結尾的 Padding
            if (padding > 0) {
                ofs.write(reinterpret_cast<char*>(paddingBytes.data()), padding);
            }
        }

        std::cout << "[Export] Saved image to: " << filename << " (" << width << "x" << height << ")" << std::endl;
        return true;
    }

} // namespace Jpeg::Decoder
