#ifndef OUTPUT_DEVICE_HPP
#define OUTPUT_DEVICE_HPP

extern "C"{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/audio_fifo.h"
    #include "libavutil/samplefmt.h"
}

class InputFile;

class OutputDevice{

    public:

        void initialize( InputFile* const input_file );
        void shutdown();

        bool fifo_contains_full_frame();

        AVFormatContext* const format_context();
        AVCodecContext* const codec_context();
        AVAudioFifo* const audio_fifo();

    private:

        static const int BIT_RATE = 96000;
        static const int CHANNELS = 2;
        static const int FRAME_SIZE = 4096;

        AVFormatContext* format_context_ = nullptr;
        AVCodecContext* codec_context_ = nullptr;
        AVAudioFifo* audio_fifo_;
};

#endif // OUTPUT_DEVICE_HPP