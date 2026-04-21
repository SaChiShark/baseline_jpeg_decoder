#include <iostream>
#include <fstream>
#include <vector>
#include "decoder/Decoder.hpp"
int main() {
    Jpeg::Decoder::JpegDecoder decoder;
    decoder.decode("../image/gig-sn01.jpg")->exportToBmp("../image/output.bmp");
    return 0;
}