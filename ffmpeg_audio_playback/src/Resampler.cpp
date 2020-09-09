#include <stdexcept>
#include <sstream>

extern "C" {
    #include "libavutil/avassert.h"
}

#include "Resampler.hpp"

// Reassign av_err2str to fix "taking address of temporary array" error in c++1x
#ifdef av_err2str
    
    #undef av_err2str

    av_always_inline char* av_err2str( int error_code ){
        static char error_string[ AV_ERROR_MAX_STRING_SIZE ];
        memset( error_string, 0, sizeof( error_string ) );

        return av_make_error_string( error_string, AV_ERROR_MAX_STRING_SIZE, error_code );
    }

#endif // av_err2str

void Resampler::initialize( InputFile* input_file, OutputDevice* output_device ){
    int error;
    std::stringstream error_sstream;

    if( 
        ( resample_context_ = swr_alloc_set_opts(
                NULL,
                av_get_default_channel_layout( output_device->codec_context()->channels ),
                output_device->codec_context()->sample_fmt,
                output_device->codec_context()->sample_rate,
                av_get_default_channel_layout( input_file->codec_context()->channels ),
                input_file->codec_context()->sample_fmt,
                input_file->codec_context()->sample_rate,
                0,
                NULL
            ) 
        ) == nullptr
    ){
        shutdown();

        error_sstream << "Could not allocate resample context.  Error: " << av_err2str( AVERROR( ENOMEM ) );
        throw std::runtime_error( error_sstream.str().c_str() );
    }

    /*
    * Perform a sanity check so that the number of converted samples is
    * not greater than the number of samples to be converted.
    * If the sample rates differ, this case has to be handled differently
    */
    av_assert0( output_device->codec_context()->sample_rate == input_file->codec_context()->sample_rate);

    if( ( error = swr_init( resample_context_ ) ) < 0 ){
        shutdown();
        error_sstream << "Could not open resample context.  Error: " << av_err2str( error ); 
        throw std::runtime_error( error_sstream.str().c_str() );
    }
}

void Resampler::shutdown(){
    if( resample_context_ != nullptr ){
        swr_free( &resample_context_ );
    }
}

SwrContext* const Resampler::resample_context(){
    return resample_context_;
}
