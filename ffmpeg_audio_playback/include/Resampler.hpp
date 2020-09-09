#ifndef RESAMPLER_HPP
#define RESAMPLER_HPP

extern "C"{
    #include "libswresample/swresample.h"
}

#include "InputFile.hpp"
#include "OutputDevice.hpp"

class Resampler{

    public:

        void initialize( InputFile* input_file, OutputDevice* output_device );
        void shutdown();

        SwrContext* const resample_context();

    private:

        SwrContext* resample_context_ = nullptr;
};

#endif // RESAMPLER_HPP