#include <sstream>
#include <stdexcept>

#include "Packet.hpp"

#include "av_err2str.hpp"

void Packet::initialize(){
    std::stringstream error_sstream;
    
    if( ( packet_ = av_packet_alloc() ) == nullptr ){
        shutdown();

        error_sstream << "Could not allocate packet. Error: " << av_err2str( AVERROR( ENOMEM ) );
        throw std::runtime_error( error_sstream.str().c_str() );
    }
     
    av_init_packet( packet_ );
    packet_->data = nullptr;
    packet_->size = 0;
}

void Packet::shutdown(){
    if( packet_ != nullptr ){
        av_packet_free( &packet_ );
    }
}

AVPacket* const Packet::packet(){
    return packet_;
}
