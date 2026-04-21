#ifndef JPEG_DECODER_HPP
#define JPEG_DECODER_HPP

#include <string>
#include <memory>
#include "decoder/StreamReader.hpp"
#include "decoder/Image.hpp"

namespace Jpeg::Decoder {

    class JpegDecoder {
    public:
        JpegDecoder();
        
        std::unique_ptr<JpegImage> decode(const std::string& filename);

    private:
        void parseDQT(StreamReader& reader, JpegImage& image);
        void parseSOF(StreamReader& reader, JpegImage& image); 
        void parseDHT(StreamReader& reader, JpegImage& image); 
        void parseSOS(StreamReader& reader, JpegImage& image); 
    };

} // namespace Jpeg::Decoder

#endif