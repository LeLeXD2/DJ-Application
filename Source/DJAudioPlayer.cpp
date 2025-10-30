/*====================================================================
DJAudioPlayer.cpp
I have created this with the help of the tutorial from this course. I update and change majority of the codes as well as add on additional features which will be labelled.
This class manages all the loading, playing and removing of the audio as well as applying filters such as speed, treble, mid and bass.
====================================================================*/


#include "DJAudioPlayer.h"
#include <juce_dsp/juce_dsp.h> 

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager) 
: formatManager(_formatManager)
{

}
DJAudioPlayer::~DJAudioPlayer()
{

}

//this function ensures that the necessary audio components are ready to process and play audio at the specified sample rate and block size
void DJAudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate) 
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    auto bassCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 100.0f, 0.707f, 0.0f);  // Low shelf filter with gainValue for bass boost
    bassFilter.coefficients = bassCoefficients;

    auto trebleCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 5000.0f, 0.707f, 5.0f);  // Low shelf filter with gainValue for bass boost
    trebleFilter.coefficients = trebleCoefficients;
}

//this function is responsible for processing and applying any audio effects to the audio data during playback
void DJAudioPlayer::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    resampleSource.getNextAudioBlock(bufferToFill);

    // Dereference the bufferToFill.buffer pointer to get the AudioBuffer
    juce::dsp::AudioBlock<float> block(*bufferToFill.buffer);  // Dereference the pointer to access the actual buffer

    // Create the context by passing the AudioBlock
    juce::dsp::ProcessContextReplacing<float> context(block);  // ProcessContextReplacing needs the block

    if (isBass) {
        //apply the bassfilter to the audio buffer
        bassFilter.process(context);
    }

    if (isTreble) {
        // Apply the treble filter to the audio buffer
        trebleFilter.process(context);
    }
    
    if (isMid) {
        // Apply the treble filter to the audio buffer
        midrangeFilter.process(context);
    }
}

//this function is used to release or clean up any resources that were previously allocated for audio playback
void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

//this function class is used to load an audio file from a given URL, and it sets up the necessary resources for playback
void DJAudioPlayer::loadURL(URL audioURL)
{
    if (audioURL.isEmpty())
    {
        transportSource.stop();

        // Reset the source (this clears the currently loaded file)
        transportSource.setSource(nullptr);
    }
    else
    {
        auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
        if (reader != nullptr) //means a good file!
        {
            std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader,
                true));
            transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            readerSource.reset(newSource.release());

        }
    }
}

//this function helps set the gain (volume level) of the audio playback
void DJAudioPlayer::setVolume(double volumeGain)
{
    if (volumeGain < 0 || volumeGain > 1.0) {
        std::cout << "volume gain should be between 0 and 1" << std::endl;
    }
    else {
        transportSource.setGain(volumeGain);
    }
}

//this function help set the speed (speed level) of the audio playback
void DJAudioPlayer::setSpeed(double speedRatio)
{
    if (speedRatio < 0 || speedRatio > 100.0) {
        std::cout << "Speed ratio should be between 0 and 100" << std::endl;
    }
    else {
        resampleSource.setResamplingRatio(speedRatio);
    }
}

//this function sets the playback position of the audio to a specific point, given in seconds.
void DJAudioPlayer::setPosition(double posSecs)
{
    transportSource.setPosition(posSecs);
}

//this function allows you to set the playback position relative to the total length of the audio track, expressed as a value between 0 and 1.
void DJAudioPlayer::setPositionRelative(double position)
{
    if (position < 0 || position > 1.0) {
        std::cout << "DJAudioPlayer::setPositionRelative pos should be between 0 and 1" << std::endl;
    }
    else {
        double posInSecs = transportSource.getLengthInSeconds() * position;
        setPosition(posInSecs);
    }
}

//this function starts the audio
void DJAudioPlayer::start()
{
    transportSource.start();
}

//this function pauses the audio
void DJAudioPlayer::stop()
{
    transportSource.stop();
}

//this function gets position of the audio
double DJAudioPlayer::getPosition()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

//this function returns the length of the audio source in seconds
double DJAudioPlayer::getLength()
{
    return transportSource.getLengthInSeconds();
}

//this function sets the trebel based on the value from the slider in DeckGUI
void DJAudioPlayer::setTreble(double gainValue)
{
    //checks if there is audio loaded
    if (readerSource == nullptr) {
        std::cout << "No audio loaded. Cannot set treble." << std::endl;
        return;
    }

    //validate the gainValue that it is within the slider value
    if (gainValue < - 0 || gainValue > 6.0)  {
        std::cout << "DJAudioPlayer::setTreble gainValue should be between -12 and +12 dB" << std::endl;
        return;
    }
    
    //variable to get the sampleRate from the audio source
    double sampleRate = readerSource->getAudioFormatReader()->sampleRate;

    //checks if sample rate is invalid
    if (sampleRate <= 0) {
        std::cout << "Invalid sample rate!" << std::endl;
        return;
    }

    //apply high-shelf filter for treble with a fixed frequency (e.g., 5000 Hz)
    float trebleFreq = 5000.0f;  //fixed treble frequency
    auto trebleCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, trebleFreq, 0.707f, static_cast<float>(gainValue));  // Use gainValue for treble boost
    trebleFilter.coefficients = trebleCoefficients;

    //changes the boolean flags for the filter
    isTreble = true;
    isBass = false;
    isMid = false;
}

//this sets the bass based on the value from the slider in DeckGUI
void DJAudioPlayer::setBass(double gainValue)
{
    //checks if there is audio loaded
    if (readerSource == nullptr) {
        std::cout << "No audio loaded. Cannot set bass." << std::endl;
        return;
    }

    //validate the gainValue that it is within the slider value
    if (gainValue < - 0 || gainValue > 6.0) {
        std::cout << "DJAudioPlayer::setBass gainValue should be between -12 and +12 dB" << std::endl;
        return;
    }

    //variable to get the sampleRate from the audio source
    double sampleRate = readerSource->getAudioFormatReader()->sampleRate;

    //checks if sample rate is invalid
    if (sampleRate <= 0) {
        std::cout << "Invalid sample rate!" << std::endl;
        return;
    }

    //apply low-shelf filter for bass with a fixed frequency
    float bassFreq = 100.0f;  //fixed bass frequency
    auto bassCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, bassFreq, 0.707f, static_cast<float>(gainValue));  // Use gainValue for bass boost
    bassFilter.coefficients = bassCoefficients;
    
    //changes the boolean flags for the filter
    isTreble = false;
    isBass = true;
    isMid = false;
}

//this sets the Mid based on the value from the slider in DeckGUI
void DJAudioPlayer::setMid(double gainValue)
{
    //checks if there is audio loaded
    if (readerSource == nullptr) {
        std::cout << "No audio loaded. Cannot set midrange." << std::endl;
        return;
    }

    //validate the gainValue that it is within the slider value
    if (gainValue < -12.0 || gainValue > 12.0)
    {
        std::cout << "DJAudioPlayer::setMidrange gainValue should be between -12 and +12 dB" << std::endl;
        return;
    }

    //variable to get the sampleRate from the audio source
    double sampleRate = readerSource->getAudioFormatReader()->sampleRate;

    //checks if sample rate is invalid
    if (sampleRate <= 0) {
        std::cout << "Invalid sample rate!" << std::endl;
        return;
    }

    //apply a bell filter (peak filter) for the midrange with a fixed frequency
    float midrangeFreq = 1000.0f; //fixed midrange frequency
    auto midrangeCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, midrangeFreq, 0.707f, static_cast<float>(gainValue));  // Use gainValue for midrange boost

    midrangeFilter.coefficients = midrangeCoefficients;

    //changes the boolean flags for the filter
    isTreble = false;
    isBass = false;
    isMid = true;
}