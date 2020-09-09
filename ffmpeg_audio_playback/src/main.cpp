#include "InputFile.hpp"
#include "OutputDevice.hpp"
#include "AudioPlayer.hpp"

int main( int argc, char* argv[] ){
    InputFile input_file;
    input_file.initialize( "data/sample.aac" );

    AudioPlayer audio_player( &input_file );
    audio_player.initialize();
    audio_player.play();
}