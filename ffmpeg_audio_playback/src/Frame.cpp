#include <stdexcept>
#include <sstream>

#include "Frame.hpp"

#include "av_err2str.hpp"

void Frame::initialize(){
    std::stringstream error_sstream;

    if( ( frame_ = av_frame_alloc() ) == nullptr ){
        shutdown();

        error_sstream << "Could not allocate input frame. Error: " << av_err2str( AVERROR( ENOMEM ) ); 
        throw std::runtime_error( error_sstream.str().c_str() );
    }
}

void Frame::shutdown(){
    if( frame_ != nullptr ){
        av_frame_free( &frame_ );
    }
}

AVFrame* const Frame::frame(){
    return frame_;
}