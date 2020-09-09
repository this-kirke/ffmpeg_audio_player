#ifndef FRAME_HPP
#define FRAME_HPP

extern "C" {
    #include "libavformat/avformat.h"
}

class Frame{

    public:

        void initialize();
        void shutdown();
        
        AVFrame* const frame();

    private:

        AVFrame* frame_;
};

#endif // FRAME_HPP