/*====================================================================
DJAudioPlayer.h
====================================================================*/


#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <juce_dsp/juce_dsp.h> 

//this class handles all the event listener for the DJplayer such as loading, playing, and manipulating audio files, with additional features like adjusting volume, speed
class DJAudioPlayer : public AudioSource {
  public:

    DJAudioPlayer(AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(URL audioURL);

    //functions that runs when user interacts with the program
    void setVolume(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);
    void setTreble(double gainValue);
    void setBass(double gainValue);
    void setMid(double gainValue);
    void start();
    void stop();

    //gets the position and length of the audio source
    double getPosition();
    double getLength();

private:
    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource; 
    ResamplingAudioSource resampleSource{&transportSource, false, 2};

    //boolean flags
    bool isTreble = false;
    bool isBass = false;
    bool isMid = false;

    //button filters
    juce::dsp::IIR::Filter<float> trebleFilter;
    juce::dsp::IIR::Filter<float> bassFilter;
    juce::dsp::IIR::Filter<float> midrangeFilter;
};




