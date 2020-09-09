#ifndef AV_ERR2STR_HPP
#define AV_ERR2STR_HPP

extern "C" {
    #include "libavutil/avutil.h"
}

// Reassign av_err2str to fix "taking address of temporary array" error in c++1x
#ifdef av_err2str
    
    #undef av_err2str

    av_always_inline char* av_err2str( int error_code ){
        static char error_string[ AV_ERROR_MAX_STRING_SIZE ];
        memset( error_string, 0, sizeof( error_string ) );

        return av_make_error_string( error_string, AV_ERROR_MAX_STRING_SIZE, error_code );
    }

#endif // av_err2str

#endif // AV_ERR2STR_HPP