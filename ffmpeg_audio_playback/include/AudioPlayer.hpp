#ifndef AUDIO_PLAYER_HPP
#define AUDIO_PLAYER_HPP

#include "InputFile.hpp"
#include "OutputDevice.hpp"
#include "Resampler.hpp"

class AudioPlayer{

    public:
        
        AudioPlayer( InputFile* input_file );
        ~AudioPlayer();

        void initialize();
        void shutdown();

        void play();

    private:

        InputFile* input_file_ = nullptr;
        OutputDevice* output_device_ = nullptr;
        Resampler* resampler_ = nullptr;
};

#endif // AUDIO_PLAYER_HPP