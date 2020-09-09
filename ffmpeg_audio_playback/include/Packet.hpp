#ifndef PACKET_HPP
#define PACKET_HPP

extern "C" {
    #include "libavcodec/avcodec.h"
}
class Packet{

    public:
        
        void initialize();
        void shutdown();

        AVPacket* const packet();

    private:

        AVPacket* packet_ = nullptr;
};

#endif // PACKET_HPP