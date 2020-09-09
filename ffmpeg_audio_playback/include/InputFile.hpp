#ifndef INPUT_FILE_HPP
#define INPUT_FILE_HPP

#include <string>

extern "C"{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
}

#include "Frame.hpp"

class InputFile{

    public:

        void initialize( std::string file_path );
        void shutdown();
        
        void read_frame( Frame* frame, bool &data_available, bool &end_of_stream );

        AVFormatContext* const format_context();
        AVCodecContext* const codec_context();

    private:
        AVFormatContext* format_context_ = nullptr;
        AVCodecContext* codec_context_ = nullptr;
};

#endif // INPUT_FILE_HPP