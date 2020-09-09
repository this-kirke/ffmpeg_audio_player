#include <sstream>

extern "C" {
    #include "libavdevice/avdevice.h"
}

#include "InputFile.hpp"
#include "OutputDevice.hpp"

// Reassign av_err2str to fix "taking address of temporary array" error in c++1x
#ifdef av_err2str
    
    #undef av_err2str

    av_always_inline char* av_err2str( int error_code ){
        static char error_string[ AV_ERROR_MAX_STRING_SIZE ];
        memset( error_string, 0, sizeof( error_string ) );

        return av_make_error_string( error_string, AV_ERROR_MAX_STRING_SIZE, error_code );
    }

#endif // av_err2str

void OutputDevice::initialize( InputFile* const input_file ){
    int error;
    std::stringstream error_sstream;

    avdevice_register_all();	

    /* Create a new format context for the output container format. */
    if( ( format_context_ = avformat_alloc_context() ) == nullptr ){
        shutdown();

        error_sstream << "Could not allocate output format context.  Error: " << av_err2str( AVERROR( ENOMEM ) );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    if( ( format_context_->oformat = av_output_audio_device_next( NULL ) ) == nullptr ){
        shutdown();

        error_sstream << "Could not find default audio output device.";
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Find the encoder to be used by its name. */
    AVCodec* output_codec = nullptr;
    if( ( output_codec = avcodec_find_encoder( format_context_->oformat->audio_codec  ) ) == nullptr ){
        shutdown();

        error_sstream << "Could not find an encoder for output audio device.";
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Create a new audio stream in the output file container. */
    AVStream *stream = nullptr;
    if( ( stream = avformat_new_stream( format_context_, NULL) ) == nullptr ){
        shutdown();

        error_sstream << "Could not create new stream.  Error: " << av_err2str( AVERROR( ENOMEM ) );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    if( ( codec_context_ = avcodec_alloc_context3( output_codec ) ) == nullptr ){
        shutdown();

        error_sstream << "Could not allocate an encoding context.  Error: " << av_err2str( AVERROR( ENOMEM ) );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Set the basic encoder parameters.
     * The input file's sample rate is used to avoid a sample rate conversion. */
    codec_context_->channels       = CHANNELS;
    codec_context_->channel_layout = av_get_default_channel_layout( CHANNELS );
    codec_context_->sample_rate    = input_file->codec_context()->sample_rate;
    codec_context_->sample_fmt     = output_codec->sample_fmts[ 0 ];
    codec_context_->bit_rate       = BIT_RATE;

    // Removed by Justin - we're not encoding aac
    /* Allow the use of the experimental AAC encoder. */
    // avcodec_context_->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    /* Set the sample rate for the container. */
    stream->time_base.den = input_file->codec_context()->sample_rate;
    stream->time_base.num = 1;

    // Removed by Justin - output device shouldn't require global header. 
    /* Some container formats (like MP4) require global headers to be present.
     * Mark the encoder so that it behaves accordingly. */
    // if( avformat_context_->oformat->flags & AVFMT_GLOBALHEADER ){
    //     avformat_context_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    // }

    /* Open the encoder for the audio stream to use it later. */
    if( ( error = avcodec_open2( codec_context_, output_codec, NULL ) ) < 0 ){
        shutdown();

        error_sstream << "Could not open output codec. Error: " << av_err2str( error );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    if( ( error = avcodec_parameters_from_context( stream->codecpar, codec_context_ ) ) < 0 ){
        shutdown();

        error_sstream << "Could not initialize stream parameters. Error: " << av_err2str( error );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Create the FIFO buffer based on the specified output sample format. */
    if( ( audio_fifo_ = av_audio_fifo_alloc( codec_context_->sample_fmt, codec_context_->channels, 1 ) ) == nullptr ){
        shutdown();

        error_sstream << "Could not allocate Audio FIFO. Error: " << av_err2str( AVERROR( ENOMEM ) );
        throw std::runtime_error( error_sstream.str().c_str() );
    }
}

void OutputDevice::shutdown(){

    if( format_context_ != nullptr ){
        avformat_free_context( format_context_ );
    }

    if( codec_context_ != nullptr ){
        avcodec_free_context( &codec_context_ );
    }

    if( audio_fifo_ != nullptr ){
        av_audio_fifo_free( audio_fifo_ );
    }
}

AVFormatContext* const OutputDevice::format_context(){
    return format_context_;
}

AVCodecContext* const OutputDevice::codec_context(){
    return codec_context_;
}

AVAudioFifo* const OutputDevice::audio_fifo(){
    return audio_fifo_;
}

bool OutputDevice::fifo_contains_full_frame(){
    return av_audio_fifo_size( audio_fifo_ ) >= FRAME_SIZE;
}