#include <iostream>
#include <stdexcept>
#include <sstream>

#include "InputFile.hpp"
#include "Packet.hpp"

#include "av_err2str.hpp"

void InputFile::initialize( std::string file_path ){
    int error;
    std::stringstream error_sstream;

    /* Open the input file to read from it. */
    if( ( avformat_open_input( &format_context_, file_path.c_str(), NULL, NULL ) ) < 0 ){
        shutdown();
        
        error_sstream << "Error opening input file.  Error: " << av_err2str( error );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Get information on the input file (number of streams etc.). */
    if( ( error = avformat_find_stream_info( format_context_, NULL ) ) < 0 ){
        shutdown();
    
        error_sstream << "Error retrieving stream info from input file  Error: " << av_err2str( error );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Make sure that there is only one stream in the input file. */
    if( format_context_->nb_streams != 1 ){
        shutdown();
    
        error_sstream << "Input file has " << format_context_->nb_streams << "streams.  Currently only a single stream is supported.";
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Find a decoder for the audio stream. */
    AVCodec *input_codec = nullptr;
    if( ( input_codec = avcodec_find_decoder( format_context_->streams[ 0 ]->codecpar->codec_id ) ) == nullptr ){
        shutdown();
    
        error_sstream << "Could not find input codec.  Error: " << av_err2str( AVERROR( AVERROR_EXIT ) );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Allocate a new decoding context. */
    if( ( codec_context_ = avcodec_alloc_context3( input_codec ) ) == nullptr ){
        shutdown();
    
        error_sstream << "Could not allocate a decoding context.  Error: " << av_err2str( AVERROR( ENOMEM ) );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Initialize the stream parameters with demuxer information. */
    if( ( error = avcodec_parameters_to_context( codec_context_, format_context_->streams[ 0 ]->codecpar ) ) < 0 ){
        shutdown();
    
        error_sstream << "Could not initialize avformat context demuxer with codec parameters.  Error: " << av_err2str( AVERROR( ENOMEM ) );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Open the decoder for the audio stream to use it later. */
    if( ( error = avcodec_open2( codec_context_, input_codec, NULL ) ) < 0 ){
        shutdown();
    
        error_sstream << "Could not open input codec.  Error: " << av_err2str( error ); 
        throw std::runtime_error( error_sstream.str().c_str() );
    }
}

void InputFile::shutdown(){
    if( codec_context_ != nullptr ){
        avcodec_free_context( &codec_context_ );
    }

    if( format_context_ ){
        avformat_close_input( &format_context_ );
    }
}

void InputFile::read_frame( Frame* frame, bool &data_available, bool &end_of_stream ){
    end_of_stream = false;
    data_available = false;

    Packet packet;
    packet.initialize();

    int error;
    std::stringstream error_sstream;

    /* Read one audio frame from the input file into a temporary packet. */
    if( ( error = av_read_frame( format_context_, packet.packet() ) ) < 0 ){
        switch( error ){
            case AVERROR_EOF:
                break;
            default:
                shutdown();
    
                error_sstream << "Could not read frame.  Error: " << av_err2str( error );
                throw std::runtime_error( error_sstream.str().c_str() );
        }
    }

    /* Send the audio frame stored in the temporary packet to the decoder.
     * The input audio stream decoder is used to do this. */
    if( ( error = avcodec_send_packet( codec_context_, packet.packet() ) ) < 0 ){
        shutdown();

        error_sstream << "Could not send packet for decoding. Error: " << av_err2str( error );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /* Receive one frame from the decoder. */
    if( error = avcodec_receive_frame( codec_context_, frame->frame() ) < 0 ){
        switch( error ){
            case AVERROR( EAGAIN ):
                data_available = false;
                break;
            case AVERROR( EOF ):
                end_of_stream = true;
                break;
            default:
                shutdown();

                error_sstream << "Could not decode frame. Error: " << av_err2str( error );
                throw std::runtime_error( error_sstream.str().c_str() );
        }
    }
}

AVFormatContext* const InputFile::format_context(){
    return format_context_;
}

AVCodecContext* const InputFile::codec_context(){
    return codec_context_;
}

