#include <sstream>
#include <stdexcept>

extern "C" {
    #include "libavutil/audio_fifo.h"
}

#include "AudioPlayer.hpp"
#include "Resampler.hpp"

#include "av_err2str.hpp"

AudioPlayer::AudioPlayer( InputFile* input_file ) : input_file_( input_file ){}

AudioPlayer::~AudioPlayer(){
    shutdown();
}

void AudioPlayer::initialize(){
    output_device_ = new OutputDevice();
    output_device_->initialize( input_file_ );

    resampler_ = new Resampler();
    resampler_->initialize( input_file_, output_device_ );
}

void AudioPlayer::shutdown(){
    if( resampler_ != nullptr ){
        delete resampler_;
    }

    if( output_device_ != nullptr ){
        delete output_device_;
    }
}

void AudioPlayer::play( ){
    int error;
    std::stringstream error_sstream;

    bool finished = false;
    while( finished == false ){

        while( output_device_->fifo_contains_full_frame() == false ){
            bool end_of_stream = false;
            bool data_present = false;

            Frame input_frame;
            input_frame.initialize();

            input_file_->read_frame( &input_frame, data_present, end_of_stream );
        }
    }
}
